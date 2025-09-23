/**
 * Démonstration de l'utilisation de l'écran LCD1602.
 *
 * Ce code utilise un module adaptateur I2C basé sur le PCF8574 pour simplifier le câblage. Dans notre kit, ce module est déjà soudé à l'arrière de l'écran LCD1602.
 * Le module I2C permet de contrôler l'écran LCD avec seulement deux fils (SDA et SCL) au lieu plusieurs fils nécessaires en mode parallèle.
 * 
 * Dans notre montage, on préfère travailler en 4 bits pour réduire le nombre de fils nécessaires.
 * L'écran LCD1602 sera donc initialisé en mode 4 bits et on devra envoyer les données en deux étapes (nibble haut puis nibble bas).
 * 1 octet = 8 bits = 2 nibbles (4 bits chacun).
 * Exemple : La commande 0x28 (0b00101000) est envoyé en deux étapes : 0b0010 = 0x2 (nibble haut) puis 0b1000 = 0x8 (nibble bas).
 * 
 * Pour la liste des commandes, il faut se référer à la documentation de l'écran LCD1602.
 * 
 * Tutoriel original : Chapitre 20 LCD1602 (Freenove)
 */

#include "lcd1602.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_random.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define I2C_SLAVE_ADDR 0x27			// Adresse I2C du module PCF8574 (souvent 0x27 ou 0x3F). Le master est l'ESP32.

#define I2C_MASTER_SCL_IO 22        // GPIO pour SCL
#define I2C_MASTER_SDA_IO 21        // GPIO pour SDA
#define I2C_MASTER_NUM I2C_NUM_0    // Numéro du port I2C. On peut avoir plusieurs ports I2C et choisir celui qu'on veut utiliser.
#define I2C_MASTER_FREQ_HZ 100000   // Fréquence de l'horloge I2C

static const char *TAG = "lcd1602.c";

static inline void lcd_send(uint8_t value, bool is_data)
{
    uint8_t hi =  (value & 0xF0);			// On masque les 4 bits de poids faible pour ne garder que le nibble haut
    uint8_t lo = ((value << 4) & 0xF0);		// On décale les 4 bits de poids faible vers la gauche et on masque les 4 bits de poids fort pour ne garder que le nibble bas

	// Le bit RS (Register Select) détermine si on envoie une commande (RS=0) ou des données (RS=1)
    uint8_t rs = is_data ? 0x01 : 0x00;

    uint8_t buf[4] = {
        (uint8_t)(hi | rs | 0x0C), // hi + RS + BL=1 (backlight à ON) + EN=1 (début de l'impulsion). On arme.
        (uint8_t)(hi | rs | 0x08), // hi + RS + BL=1 (backlight à ON) + EN=0 (fin de l'impulsion). On déclenche.
        (uint8_t)(lo | rs | 0x0C), // lo + RS + BL=1 (backlight à ON) + EN=1 (début de l'impulsion). On arme.
        (uint8_t)(lo | rs | 0x08), // lo + RS + BL=1 (backlight à ON) + EN=0 (fin de l'impulsion). On déclenche.
    };

	i2c_master_write_to_device(I2C_MASTER_NUM, I2C_SLAVE_ADDR, buf, 4, pdMS_TO_TICKS(100));
}

void lcd_send_cmd(uint8_t cmd) { lcd_send(cmd, false); }
void lcd_send_data(uint8_t ch) { lcd_send(ch, true); }

void lcd_put_cur(int row, int col)
{
	// Validation ...
    if (row < 0) row = 0;
    if (row > 1) row = 1;
    if (col < 0) col = 0;
    if (col > 15) col = 15;

    uint8_t base = (row == 0) ? 0x00 : 0x40;   // On choisit la ligne (0 ou 1)
    uint8_t addr = base + (uint8_t)col;        // On calcule l'adresse DDRAM (0..0x4F)
    lcd_send_cmd(0x80 | addr);                 // On envoie la commande pour positionner le curseur (0x80 + adresse)
}

void lcd1602_init (void)
{
	// Les appels à esp_rom_delay_us sont nécessaires pour respecter les temps d'attente entre les commandes.
	// esp_rom_delay_us bloque le thread pendant le nombre de microsecondes spécifié.
	// Si le délai est long, on peut utiliser vTaskDelay(pdMS_TO_TICKS(ms)) pour libérer le CPU.
	// Cependant, pour les délais très courts (quelques microsecondes), esp_rom_delay_us est préférable car plus précis.

	// Voir la documentation de l'écran LCD1602 pour les détails sur les délais.

	// Un peut bizarre mais le fabricant demande de lancer la commande 0x30 trois fois pour s'assurer que l'écran est en mode 8 bits.
	// Ensuite, on passe en mode 4 bits avec la commande 0x20.
	esp_rom_delay_us(50000);
	lcd_send_cmd (0x30);
	esp_rom_delay_us(5000);
	lcd_send_cmd (0x30);
	esp_rom_delay_us(200);
	lcd_send_cmd (0x30);
	esp_rom_delay_us(10000);
	lcd_send_cmd (0x20); // Passage en mode 4 bits
	
	esp_rom_delay_us(10000);

	lcd_send_cmd (0x28); // Fonction : 4 bits, 2 lignes, 5x8 points 
	esp_rom_delay_us(1000);

	lcd_send_cmd (0x08); // On éteint l'affichage
	esp_rom_delay_us(1000);

	lcd_send_cmd (0x01); // On efface l'affichage
	esp_rom_delay_us(2000);

	lcd_send_cmd (0x06); // Mode d'entrée : incrément, pas de décalage
	esp_rom_delay_us(1000);

	lcd_send_cmd (0x0C); // Affichage : allumé, curseur éteint, clignotement éteint
	esp_rom_delay_us(1000);
}

static void i2c_master_init(void)
{
    // Configuration du Master I2C
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &i2c_config);
    i2c_driver_install(I2C_MASTER_NUM, i2c_config.mode, 0, 0, 0);
}


void lcd_clear (void)
{
	lcd_send_cmd (0x01);
	esp_rom_delay_us(5000);
}

void lcd_send_string (const char *str)
{
	while (*str) lcd_send_data (*str++);
}

static void lcd1602_task(void *arg)
{
    while(1) {
		// Affichage de texte statique
		ESP_LOGI(TAG, "Affichage sur l'écran LCD1602 - Ligne 1 et 2");
		lcd_clear();
		lcd_put_cur(0, 0); // Ligne 0, Colonne 0
        lcd_send_string("Super test !");

        lcd_put_cur(1, 0); // Ligne 1, Colonne 0

		int random_value = esp_random() % 100; // valeur random pour tester
		char buf[16];
		snprintf(buf, sizeof(buf), "Valeur=%d", random_value);
		lcd_send_string(buf);

        vTaskDelay(pdMS_TO_TICKS(2000));

		// Affichage de texte défilant
		ESP_LOGI(TAG, "Affichage sur l'écran LCD1602 - Défilement");
		char msg[] = "Super test plus long !";
		int len = strlen(msg);
		int steps = len > 16 ? (len - 16) : 0;

		lcd_clear();
		lcd_put_cur(0, 0);
		lcd_send_string(msg);

		for (int pos = 0; pos < steps; pos++) {
			lcd_send_cmd(0x18); // shift vers la gauche
			vTaskDelay(pdMS_TO_TICKS(300));
		}

		vTaskDelay(pdMS_TO_TICKS(2000));

		// Affichage de texte défilant sur une ligne et texte fixe sur l'autre
		ESP_LOGI(TAG, "Affichage sur l'écran LCD1602 - Ligne 1 et 2 - Défilement");
		lcd_clear();

        lcd_put_cur(1, 0);
		lcd_send_string("ligne fixe");

		for (int pos = 0; pos < steps + 1; pos++) {
			lcd_put_cur(0, 0);
			char buf[17];
			strncpy(buf, &msg[pos], 16);
			buf[16] = '\0';
			lcd_send_string(buf);
			vTaskDelay(pdMS_TO_TICKS(300));
		}

		vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void start_demo_lcd1602_task(void)
{
    i2c_master_init();
    lcd1602_init();

    lcd_clear();

    xTaskCreate(lcd1602_task, "lcd1602_task", 2048, NULL, 5, NULL);
}