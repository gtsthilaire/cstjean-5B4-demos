#ifndef LED_BAR_74HC595_H
#define LED_BAR_74HC595_H

#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio_data;
    gpio_num_t gpio_clock;
    gpio_num_t gpio_latch;
} led_bar_74hc595_t;

// Configure les 3 GPIO du 74HC595 en sortie, éteint toutes les LED et retourne un led_bar_74hc595_t.
led_bar_74hc595_t led_bar_74hc595_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch);

// Allume les LED selon les bits de leds : chaque bit correspond à une LED (1 = allumée).
// Ex : 0x01 = 00000001 allume seulement la première LED, 0xFF = 11111111 les allume toutes.
void led_bar_74hc595_show(led_bar_74hc595_t *led_bar_74hc595, uint8_t leds);

#endif // LED_BAR_74HC595_H
