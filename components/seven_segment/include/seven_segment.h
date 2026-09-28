#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio_data;
    gpio_num_t gpio_clock;
    gpio_num_t gpio_latch;
} seven_segment_t;

// Configure les 3 GPIO du 74HC595 en sortie, éteint l'afficheur et retourne un seven_segment_t.
seven_segment_t seven_segment_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch);

// Affiche un chiffre de 0 à 15 (0 à 9, puis A, b, C, d, E, F). Hors de cette plage, l'afficheur s'éteint.
void seven_segment_show_digit(seven_segment_t *seven_segment, int digit);

#endif // SEVEN_SEGMENT_H
