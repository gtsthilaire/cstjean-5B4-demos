/**
 * Ce code utilise un module adaptateur I2C basé sur le PCF8574 pour simplifier le câblage. Dans notre kit, ce module est déjà soudé à l'arrière de l'écran LCD1602.
 * Le module I2C permet de contrôler l'écran LCD avec seulement deux fils (SDA et SCL) au lieu de plusieurs fils nécessaires en mode parallèle.
 *
 * Dans notre montage, on préfère travailler en 4 bits pour réduire le nombre de fils nécessaires.
 * L'écran LCD1602 sera donc initialisé en mode 4 bits et on devra envoyer les données en deux étapes (nibble haut puis nibble bas).
 *
 * Un nibble est un demi-octet : 4 bits (« nibble » veut dire « petite bouchée », un octet étant un « byte », une « bouchée »).
 * 1 octet = 8 bits = 2 nibbles. Un nibble se lit avec un seul chiffre hexadécimal (0x0 à 0xF), ce qui rend la découpe facile à visualiser :
 * le premier chiffre hexadécimal d'un octet est son nibble haut, le deuxième est son nibble bas.
 *
 *   octet 0x28 = 0b0010 1000
 *                  |     |
 *                  |     +-- nibble bas  = 0b1000 = 0x8
 *                  +-------- nibble haut = 0b0010 = 0x2
 *
 * L'écran possède 8 lignes de données (D0 à D7), mais en mode 4 bits seules D4 à D7 sont reliées.
 * On ne peut donc envoyer que 4 bits à la fois : pour transmettre un octet (une commande ou un caractère), on envoie d'abord
 * le nibble haut, puis le nibble bas. La fonction lcd_send() ci-dessous fait exactement ça.
 *
 * Pour la liste des commandes, il faut se référer à la documentation de l'écran LCD1602.
 */

#include "lcd1602.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include <stdbool.h>

static const char *TAG = "components/lcd1602";

#define I2C_MASTER_NUM I2C_NUM_0  // Numéro du port I2C. On peut avoir plusieurs ports I2C et choisir celui qu'on veut utiliser.
#define I2C_MASTER_FREQ_HZ 100000 // Fréquence de l'horloge I2C
#define I2C_TIMEOUT_MS 100

static void lcd_send(lcd1602_t *lcd, uint8_t value, bool is_data)
{
    uint8_t hi =  (value & 0xF0);        // On masque les 4 bits de poids faible pour ne garder que le nibble haut
    uint8_t lo = ((value << 4) & 0xF0);  // On décale les 4 bits de poids faible vers la gauche et on masque les 4 bits de poids fort pour ne garder que le nibble bas

    // Le bit RS (Register Select) détermine si on envoie une commande (RS=0) ou des données (RS=1)
    uint8_t rs = is_data ? 0x01 : 0x00;

    // Chaque octet envoyé au PCF8574 pilote directement ses 8 sorties, qui sont reliées à l'écran de cette façon :
    //
    //   bit :   7   6   5   4  |  3    2    1    0
    //          D7  D6  D5  D4  | BL   EN   RW   RS
    //          └─ le nibble ─┘    │    │    │    └─ RS : commande (0) ou donnée (1)
    //             à envoyer       │    │    └────── RW : toujours 0 (on écrit seulement)
    //                             │    └─────────── EN : impulsion qui dit à l'écran « lis maintenant »
    //                             └──────────────── BL : rétroéclairage
    //
    // Un octet du LCD (ex: 'A' = 0x41, hi = 0x4, lo = 0x1) donne donc 4 octets I2C : 0x4D, 0x49, 0x1D, 0x19.
    // Chaque nibble est envoyé deux fois : une fois avec EN=1, puis avec EN=0. C'est la chute de EN qui déclenche la lecture.
    uint8_t buf[4] = {
        (uint8_t)(hi | rs | 0x0C), // hi + RS + BL=1 (backlight à ON) + EN=1 (début de l'impulsion). On arme.
        (uint8_t)(hi | rs | 0x08), // hi + RS + BL=1 (backlight à ON) + EN=0 (fin de l'impulsion). On déclenche.
        (uint8_t)(lo | rs | 0x0C), // lo + RS + BL=1 (backlight à ON) + EN=1 (début de l'impulsion). On arme.
        (uint8_t)(lo | rs | 0x08), // lo + RS + BL=1 (backlight à ON) + EN=0 (fin de l'impulsion). On déclenche.
    };

    i2c_master_transmit(lcd->i2c_dev, buf, sizeof(buf), I2C_TIMEOUT_MS);
}

static void lcd_send_cmd(lcd1602_t *lcd, uint8_t cmd) { lcd_send(lcd, cmd, false); }
static void lcd_send_data(lcd1602_t *lcd, uint8_t ch) { lcd_send(lcd, ch, true); }

lcd1602_t lcd1602_init(gpio_num_t sda, gpio_num_t scl, uint8_t i2c_addr)
{
    ESP_LOGI(TAG, "Configuration du bus I2C (SDA: GPIO %d, SCL: GPIO %d) et du module à l'adresse 0x%02X", sda, scl, i2c_addr);

    // Configuration du bus I2C. L'ESP32 est le master.
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7, // Valeur typique : filtre les parasites très courts sur les lignes
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    i2c_new_master_bus(&bus_config, &bus);

    // Ajout du module PCF8574 (le slave) sur le bus.
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    lcd1602_t lcd = { .i2c_dev = NULL };
    i2c_master_bus_add_device(bus, &dev_config, &lcd.i2c_dev);

    // Les appels à esp_rom_delay_us sont nécessaires pour respecter les temps d'attente entre les commandes.
    // esp_rom_delay_us bloque le thread pendant le nombre de microsecondes spécifié.
    // Si le délai est long, on peut utiliser vTaskDelay(pdMS_TO_TICKS(ms)) pour libérer le CPU.
    // Cependant, pour les délais très courts (quelques microsecondes), esp_rom_delay_us est préférable car plus précis.

    // Voir la documentation de l'écran LCD1602 pour les détails sur les délais.

    // Un peu bizarre mais le fabricant demande de lancer la commande 0x30 trois fois pour s'assurer que l'écran est en mode 8 bits.
    // Ensuite, on passe en mode 4 bits avec la commande 0x20.
    esp_rom_delay_us(50000);
    lcd_send_cmd(&lcd, 0x30);
    esp_rom_delay_us(5000);
    lcd_send_cmd(&lcd, 0x30);
    esp_rom_delay_us(200);
    lcd_send_cmd(&lcd, 0x30);
    esp_rom_delay_us(10000);
    lcd_send_cmd(&lcd, 0x20); // Passage en mode 4 bits

    esp_rom_delay_us(10000);

    lcd_send_cmd(&lcd, 0x28); // Fonction : 4 bits, 2 lignes, 5x8 points
    esp_rom_delay_us(1000);

    lcd_send_cmd(&lcd, 0x08); // On éteint l'affichage
    esp_rom_delay_us(1000);

    lcd_send_cmd(&lcd, 0x01); // On efface l'affichage
    esp_rom_delay_us(2000);

    lcd_send_cmd(&lcd, 0x06); // Mode d'entrée : incrément, pas de décalage
    esp_rom_delay_us(1000);

    lcd_send_cmd(&lcd, 0x0C); // Affichage : allumé, curseur éteint, clignotement éteint
    esp_rom_delay_us(1000);

    return lcd;
}

void lcd1602_clear(lcd1602_t *lcd)
{
    if (lcd == NULL) {
        return;
    }

    lcd_send_cmd(lcd, 0x01);
    esp_rom_delay_us(2000); // La commande d'effacement est la plus lente (~1,5 ms)
}

void lcd1602_set_cursor(lcd1602_t *lcd, int row, int col)
{
    if (lcd == NULL) {
        return;
    }

    // Validation ...
    if (row < 0) row = 0;
    if (row > LCD1602_ROWS - 1) row = LCD1602_ROWS - 1;
    if (col < 0) col = 0;
    if (col > LCD1602_COLS - 1) col = LCD1602_COLS - 1;

    uint8_t base = (row == 0) ? 0x00 : 0x40; // On choisit la ligne (0 ou 1)
    uint8_t addr = base + (uint8_t)col;      // On calcule l'adresse DDRAM (0..0x4F)
    lcd_send_cmd(lcd, 0x80 | addr);          // On envoie la commande pour positionner le curseur (0x80 + adresse)
}

void lcd1602_print(lcd1602_t *lcd, const char *str)
{
    if (lcd == NULL || str == NULL) {
        return;
    }

    while (*str) {
        lcd_send_data(lcd, *str++);
    }
}

void lcd1602_scroll_left(lcd1602_t *lcd)
{
    if (lcd == NULL) {
        return;
    }

    lcd_send_cmd(lcd, 0x18); // Décalage de l'affichage vers la gauche
}
