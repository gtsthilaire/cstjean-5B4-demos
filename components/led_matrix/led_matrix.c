/**
 * Matrice de 8x8 LED contrôlée avec deux 74HC595.
 *
 * Un 74HC595 permet de contrôler plusieurs sorties numériques en utilisant seulement 3 broches GPIO.
 * Dans notre cas, nous en utilisons deux (branchés en cascade) pour contrôler 64 LED. Un pour les rangées et
 * l'autre pour les colonnes.
 *
 * Le 74HC595 est un registre à décalage (shift register). Il permet de convertir des données série en données
 * parallèles. Il fonctionne comme une file de bits : on lui envoie un bit à la fois, il décale les précédents
 * et au bout d'un moment (8 bits), on a une série de bits stockés à l'intérieur afin de les utiliser.
 * Quand on envoie plus de 8 bits, les bits qui « débordent » passent au deuxième 74HC595.
 *
 * Il fonctionne avec 3 GPIO :
 * - Données (Data) : les bits sont envoyés un par un sur cette broche. Ils sont stockés dans le registre.
 * - Horloge (Clock) : chaque impulsion fait entrer le bit présent sur Data dans le registre.
 * - Verrou (Latch) : il copie le contenu du registre sur les sorties (les LED changent à ce moment-là).
 *
 * Dans la matrice, chaque LED a son anode (côté positif) connectée à une rangée et sa cathode (côté négatif)
 * connectée à une colonne. Pour allumer une LED spécifique, on doit activer la rangée correspondante
 * (mettre l'anode à HIGH) et activer la colonne correspondante (mettre la cathode à LOW) pour faire circuler
 * le courant.
 *
 * Comme on peut seulement allumer une colonne à la fois, on utilise une technique de balayage qui consiste à
 * activer chaque colonne successivement, très rapidement, tout en affichant sur chaque colonne les
 * rangées correspondantes. Avec la vitesse, notre cerveau perçoit toutes les LED comme allumées en même temps.
 *
 * Écrire un motif en octets (un par colonne) n'est pas très lisible. On peut donc aussi le dessiner en texte,
 * ligne par ligne, avec '#' pour une LED allumée : led_matrix_show_drawing s'occupe de la conversion.
 */

#include "led_matrix.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "components/led_matrix";

#define LED_MATRIX_REFRESH_US 1000 // Durée d'affichage de chaque colonne (µs). 8 colonnes → toute la matrice en 8 ms.

// Envoie un octet au 74HC595, bit par bit, en commençant par le bit de poids fort.
static void shift_out(led_matrix_t *led_matrix, uint8_t value)
{
    for (int i = 7; i >= 0; i--) {
        gpio_set_level(led_matrix->gpio_data, (value >> i) & 0x01); // On place le bit i sur Data (le & 0x01 ne garde que ce bit)
        gpio_set_level(led_matrix->gpio_clock, 1);                  // Front montant : le registre lit le bit
        gpio_set_level(led_matrix->gpio_clock, 0);                  // On prépare la prochaine impulsion
    }
}

// Envoie les rangées puis les colonnes, et les affiche.
// rows : chaque bit représente une rangée (1 = allumée)
// cols : chaque bit représente une colonne (0 = active, car on met la cathode à LOW)
static void display(led_matrix_t *led_matrix, uint8_t rows, uint8_t cols)
{
    gpio_set_level(led_matrix->gpio_latch, 0); // On désactive le latch pour commencer à envoyer les données
    shift_out(led_matrix, rows);
    shift_out(led_matrix, cols);
    gpio_set_level(led_matrix->gpio_latch, 1); // On active le latch pour que les données soient affichées
}

// Appelée périodiquement par le timer : affiche la colonne courante puis passe à la suivante.
//
// Exemple pour le visage souriant, colonne 0 : 0x1C = 00011100
//     7 6 5 4 3 2 1 0
// 7   · · · · · · · ·
// 6   · · · · · · · ·
// 5   · · · · · · · ·
// 4   · · · · · · · ●
// 3   · · · · · · · ●
// 2   · · · · · · · ●
// 1   · · · · · · · ·
// 0   · · · · · · · ·
static void refresh_timer_callback(void *arg)
{
    led_matrix_t *led_matrix = (led_matrix_t *)arg;
    int col = led_matrix->current_col;

    uint8_t rows = led_matrix->pattern[col];

    // On crée un octet avec seulement le bit de la colonne courante à 0.
    // Exemple pour la colonne 0 : 11111110
    uint8_t cols = (uint8_t)~(1u << col);

    display(led_matrix, rows, cols);

    led_matrix->current_col = (col + 1) % LED_MATRIX_SIZE;
}

led_matrix_t led_matrix_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch)
{
    ESP_LOGI(TAG, "Configuration des GPIO %d (data), %d (clock) et %d (latch) en sortie", gpio_data, gpio_clock, gpio_latch);

    // On configure les 3 broches d'un coup avec un masque de bits
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << gpio_data) | (1ULL << gpio_clock) | (1ULL << gpio_latch),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_config);

    led_matrix_t led_matrix = {
        .gpio_data = gpio_data,
        .gpio_clock = gpio_clock,
        .gpio_latch = gpio_latch,
        .pattern = { 0 },
        .current_col = 0,
        .refresh_timer = NULL,
    };

    gpio_set_level(gpio_latch, 0);
    gpio_set_level(gpio_clock, 0);
    gpio_set_level(gpio_data, 0);

    display(&led_matrix, 0x00, 0xFF); // On commence avec toutes les LED éteintes

    return led_matrix;
}

void led_matrix_start(led_matrix_t *led_matrix)
{
    if (led_matrix == NULL || led_matrix->refresh_timer != NULL) {
        return;
    }

    // Le timer reçoit l'adresse de led_matrix : la variable doit donc exister tant que le balayage tourne
    // (ex : variable static, comme dans la démo).
    const esp_timer_create_args_t timer_args = {
        .callback = &refresh_timer_callback,
        .arg = led_matrix,
        .name = "led_matrix_refresh",
    };
    esp_timer_create(&timer_args, &led_matrix->refresh_timer);
    esp_timer_start_periodic(led_matrix->refresh_timer, LED_MATRIX_REFRESH_US);

    ESP_LOGI(TAG, "Balayage démarré");
}

void led_matrix_show(led_matrix_t *led_matrix, const uint8_t pattern[LED_MATRIX_SIZE])
{
    if (led_matrix == NULL || pattern == NULL) {
        return;
    }

    // Le timer peut afficher une colonne pendant la copie : au pire, une colonne de l'ancien motif reste visible
    // pendant 1 ms, ce qui est invisible à l'œil.
    memcpy(led_matrix->pattern, pattern, LED_MATRIX_SIZE);
}

void led_matrix_show_drawing(led_matrix_t *led_matrix, const char *const drawing[LED_MATRIX_SIZE])
{
    if (drawing == NULL) {
        return;
    }

    // On convertit le dessin en octets (un par colonne), comme ceux attendus par led_matrix_show :
    // - la première ligne du dessin est la rangée du haut (rangée 7), la dernière est la rangée du bas (rangée 0) ;
    // - la colonne de gauche du dessin est la colonne 7, celle de droite est la colonne 0.
    uint8_t pattern[LED_MATRIX_SIZE] = { 0 };
    for (int line = 0; line < LED_MATRIX_SIZE; line++) {
        for (int x = 0; x < LED_MATRIX_SIZE; x++) {
            if (drawing[line][x] == '#') {
                pattern[7 - x] |= 1 << (7 - line); // on allume le bit de la rangée dans l'octet de la colonne
            }
        }
    }

    led_matrix_show(led_matrix, pattern);
}
