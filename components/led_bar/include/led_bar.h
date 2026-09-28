#ifndef LED_BAR_H
#define LED_BAR_H

#include <stdbool.h>
#include <stddef.h>
#include "driver/gpio.h"

#define LED_BAR_MAX_LEDS 10 // La barre de LED du kit Freenove contient 10 LED

typedef struct {
    gpio_num_t gpios[LED_BAR_MAX_LEDS];
    size_t count;
} led_bar_t;

// Configure chaque GPIO en sortie et retourne un led_bar_t. Toutes les LED démarrent éteintes.
// Le nombre de LED est limité à LED_BAR_MAX_LEDS.
led_bar_t led_bar_init(const gpio_num_t *gpios, size_t count);

// Allume (true) ou éteint (false) la LED à l'index donné (0 = première LED).
void led_bar_set(led_bar_t *led_bar, size_t index, bool on);

// Allume les `level` premières LED et éteint les autres (0 = tout éteint, count = tout allumé).
// Pratique pour afficher une valeur sous forme de bargraph (ex : un niveau sonore ou la position d'un potentiomètre).
void led_bar_set_level(led_bar_t *led_bar, size_t level);

#endif // LED_BAR_H
