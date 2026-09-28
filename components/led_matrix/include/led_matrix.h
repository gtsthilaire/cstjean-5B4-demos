#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>
#include "driver/gpio.h"
#include "esp_timer.h"

#define LED_MATRIX_SIZE 8 // Matrice de 8x8 LED

typedef struct {
    gpio_num_t gpio_data;
    gpio_num_t gpio_clock;
    gpio_num_t gpio_latch;
    uint8_t pattern[LED_MATRIX_SIZE]; // motif affiché : un octet par colonne, chaque bit = une rangée
    int current_col;                  // colonne en cours d'affichage (0..7), avancée à chaque rafraîchissement
    esp_timer_handle_t refresh_timer;
} led_matrix_t;

// Configure les 3 GPIO du 74HC595 en sortie, éteint toutes les LED et retourne un led_matrix_t.
led_matrix_t led_matrix_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch);

// Démarre le balayage périodique des colonnes (via esp_timer). Sans balayage, rien ne s'affiche.
void led_matrix_start(led_matrix_t *led_matrix);

// Change le motif affiché (8 octets, un par colonne).
void led_matrix_show(led_matrix_t *led_matrix, const uint8_t pattern[LED_MATRIX_SIZE]);

// Change le motif affiché à partir d'un dessin : 8 lignes de texte (de haut en bas), '#' = LED allumée.
// Ex : "..####.." allume les 4 LED du milieu de la rangée.
void led_matrix_show_drawing(led_matrix_t *led_matrix, const char *const drawing[LED_MATRIX_SIZE]);

#endif // LED_MATRIX_H
