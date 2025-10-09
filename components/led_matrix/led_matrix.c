/**
 * Démonstration de l'utilisation d'une matrice de LEDs avec deux 74HC595.
 * 
 * Un 74HC595 permet de contrôler plusieurs sorties numériques en utilisant seulement 3 broches GPIO. 
 * Dans notre cas, nous en utilisons deux pour contrôler 64 LEDs. Un pour les lignes et l'autre pour les colonnes.
 * 
 * Le 74HC595 est un registre à décalage (shift register). Il permet de convertir des données série en données 
 * parallèles. Il fonctionne comme une file de bits : on lui envoie un bit à la fois, il décale les précédents 
 * et au bout d’un moment (8 bits), on a une série de bits stockés à l’intérieur afin de les utiliser.
 * 
 * Il fonctionne avec 3 GPIOs :
 * - Un GPIO pour les données (Data) : les bits sont envoyés un par un sur cette broche. elles 
 *                                      sont stockées dans le registre.
 * - Un GPIO pour l'horloge (Clock) : pour la synchronisation des données
 * - Un GPIO pour le latch (Latch) : il permet d'envoyer les données stockées dans le registre
 * 
 * Dans la matrice, chaque LED a son anode (côté positif) connectée à une ligne et sa cathode (côté négatif) 
 * connectée à une colonne. Pour allumer une LED spécifique, on doit activer la ligne correspondante 
 * (mettre l'anode à HIGH) et désactiver la colonne correspondante (mettre la cathode à LOW) pour faire circuler 
 * le courant.
 * 
 * Comme on peut seulement allumer une colonne à la fois, on utilise une technique de balayage qui consiste à 
 * activer chaque colonne successivement, très rapidement, tout en affichant sur chaque colonne les 
 * rangées correspondantes. Avec la vitesse, notre cerveau perçoit toutes les LEDs comme allumées en même temps.
 * 
 * Tutoriel original : Chapitre 16 - 74HC595 & LED Matrix
 */

#include "led_matrix.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "led_matrix.c";

static gpio_num_t s_data_gpio = -1;
static gpio_num_t s_clock_gpio = -1;
static gpio_num_t s_latch_gpio = -1;

// Exemple de visage souriant (smiling face)
//
//     7 6 5 4 3 2 1 0
// 7   · · · · · · · ·
// 6   · · ● ● ● ● · ·
// 5   · ● · · · · ● ·
// 4   ● · ● · · ● · ●
// 3   ● · · · · · · ●
// 2   ● · · ● ● · · ●
// 1   · ● · · · · ● ·
// 0   · · ● ● ● ● · ·
//
// Ceci correspond donc aux colonnes de la matrice de LEDs
const uint8_t smilingFace [] = {
    0x1C,   // 00011100 = · · · ● ● ● · ·
    0x22,   // 00100010 = · · ● · · · ● ·
    0x51,   // 01010001 = · ● · ● · · · ●
    0x45,   // 01000101 = · ● · · · ● · ●
    0x45,   // 01000101 = · ● · · · ● · ●
    0x51,   // 01010001 = · ● · ● · · · ●
    0x22,   // 00100010 = · · ● · · · ● ·
    0x1C,   // 00011100 = · · · ● ● ● · ·
};

// D'autres patterns juste pour montrer qu'on peut afficher autre chose ...
const uint8_t lettreA[] = {
    0x00,   // 00000000 = · · · · · · · ·
    0x00,   // 00000000 = · · · · · · · ·
    0x3F,   // 00111111 = · · ● ● ● ● ● ●
    0x44,   // 01000100 = · ● · · · ● · ·
    0x44,   // 01000100 = · ● · · · ● · ·
    0x3F,   // 00111111 = · · ● ● ● ● ● ●
    0x00,   // 00000000 = · · · · · · · ·
    0x00,   // 00000000 = · · · · · · · ·
};

const uint8_t lettreB[] = {
    0x00,   // 00000000 = · · · · · · · ·
    0x00,   // 00000000 = · · · · · · · ·
    0x36,   // 00110110 = · · ● ● · ● ● ·
    0x49,   // 01001001 = · ● · · ● · · ●
    0x49,   // 01001001 = · ● · · ● · · ●
    0x7F,   // 01111111 = · ● ● ● ● ● ● ●
    0x00,   // 00000000 = · · · · · · · ·
    0x00,   // 00000000 = · · · · · · · ·
};

const uint8_t *patterns[] = {
    smilingFace,
    lettreA,
    lettreB
};

static volatile int s_current_pattern = 0; // Index du motif courant dans le tableau patterns
static volatile int s_current_col = 0; // Colonne courante (0 à 7)

static void led_matrix_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch)
{
    s_data_gpio = gpio_data;
    s_clock_gpio = gpio_clock;
    s_latch_gpio = gpio_latch;

    // On doit configurer les GPIOs utilisés pour le 74HC595
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << s_data_gpio) | (1ULL << s_clock_gpio) | (1ULL << s_latch_gpio), // Pour configurer les 3 broches d'un coup
        .mode = GPIO_MODE_OUTPUT
    };

    gpio_config(&io_conf);

    gpio_set_level(s_latch_gpio, 0);
    gpio_set_level(s_clock_gpio, 0);
    gpio_set_level(s_data_gpio,  0);
}

// Fonction pour envoyer les données au 74HC595
// Le paramètre pattern est un octet où chaque bit représente l'état d'une LED dans une colonne (chaque bit = une rangée)
// Le paramètre cols est un octet où chaque bit représente l'état d'une colonne (chaque bit = une colonne)
static void afficherDonnees(uint8_t pattern, uint8_t cols) 
{ 
    gpio_set_level(s_latch_gpio, 0); // On désactive le latch pour commencer à envoyer les données

    // On envoie les 8 bits du pattern un par un. Correspond à chaque rangée.
    for (int i = 7; i >= 0; i--) {
        gpio_set_level(s_data_gpio, (pattern >> i) & 0b00000001); // On envoie le bit i. Le & 0b00000001 permet de ne garder que le bit de poids faible (le bit i)
        gpio_set_level(s_clock_gpio, 1); // On active l'horloge pour que le registre prenne en compte la donnée
        gpio_set_level(s_clock_gpio, 0); // On désactive l'horloge pour préparer la prochaine donnée
    }

    // On envoie les 8 bits un par un pour activer les colonnes correspondantes.
    for (int i = 7; i >= 0; i--) {
        gpio_set_level(s_data_gpio, (cols >> i) & 0b00000001);
        gpio_set_level(s_clock_gpio, 1);
        gpio_set_level(s_clock_gpio, 0);
    }

    gpio_set_level(s_latch_gpio, 1); // On active le latch pour que les données soient prises en compte et affichées 
}

// On active chaque rangée pour la colonne courante.
// Exemple pour la première colonne (s_current_col=7) : 00011100
//     7 6 5 4 3 2 1 0
// 7   · · · · · · · ·
// 6   · · · · · · · ·
// 5   · · · · · · · ·
// 4   ● · · · · · · ·
// 3   ● · · · · · · ·
// 2   ● · · · · · · ·
// 1   · · · · · · · ·
// 0   · · · · · · · ·
//
// Cette fonction est appelée périodiquement par le timer.
static void matrix_refresh_callback(void *arg)
{
    // Copie locale des variables volatiles pour éviter les problèmes d'accès concurrent
    const int current_col = s_current_col;
    const uint8_t *pattern = patterns[s_current_pattern];
    
    uint8_t rows = pattern[current_col];
    
    // On crée un octet avec seulement le bit de la colonne courante à 0.
    // Exemple pour la première colonne (s_current_col=0) : 11111110
    // on doit inverser les bits car on met la cathode à LOW pour faire sortir le courant.
    uint8_t cols = (uint8_t)~(1u << current_col);

    // On active les rangées pour la colonne s_current_col.
    afficherDonnees(rows, cols);

    // On passe à la colonne suivante
    s_current_col++;
    if (s_current_col >= 8) s_current_col = 0;
}

static void motif_toggle_task(void *arg)
{
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(5000));

        // On change de motif pour la démo ...
        ESP_LOGI(TAG, "Changement de motif...");
        s_current_pattern++;
        if (s_current_pattern >= sizeof(patterns) / sizeof(patterns[0])) {
            s_current_pattern = 0;
        }
    }
}

void start_demo_led_matrix_timer(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch)
{
    led_matrix_init(gpio_data, gpio_clock, gpio_latch);

    afficherDonnees(0x00, 0xFF); // On commence avec toutes les LEDs éteintes

    const esp_timer_create_args_t timer_args = {
        .callback = &matrix_refresh_callback,
        .name = "matrix_refresh"
    };

    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);

    // Le délai permet de voir les LEDs allumées avant de passer à la colonne suivante
    esp_timer_start_periodic(timer, 1000); // µs

    // Tâche pour changer le motif toutes les 5 secondes
    xTaskCreate(motif_toggle_task, "motif_toggle_task", 2048, NULL, 5, NULL);
}