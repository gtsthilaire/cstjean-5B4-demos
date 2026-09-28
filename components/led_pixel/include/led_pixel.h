#ifndef LED_PIXEL_H
#define LED_PIXEL_H

#include <stddef.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "led_strip.h"

typedef struct {
    led_strip_handle_t strip; // module WS2812 (géré par le composant led_strip d'Espressif)
    size_t count;             // nombre de LED dans le module
    uint8_t brightness;       // luminosité globale (0 = éteint, 255 = maximum)
} led_pixel_t;

// Configure le module WS2812 (via le périphérique RMT) sur le GPIO donné et retourne un led_pixel_t.
// Les LED démarrent éteintes, avec une luminosité globale au maximum (255).
led_pixel_t led_pixel_init(gpio_num_t gpio, size_t count);

// Règle la luminosité globale (0..255). Elle s'applique aux prochains appels de led_pixel_set et led_pixel_set_hsv.
void led_pixel_set_brightness(led_pixel_t *led_pixel, uint8_t brightness);

// Change la couleur d'une LED (0 = première LED). Le changement n'est visible qu'après led_pixel_show.
void led_pixel_set(led_pixel_t *led_pixel, size_t index, uint8_t r, uint8_t g, uint8_t b);

// Change la couleur d'une LED selon sa teinte sur la roue des couleurs (0..359 : 0 = rouge, 120 = vert, 240 = bleu).
// Le changement n'est visible qu'après led_pixel_show.
void led_pixel_set_hsv(led_pixel_t *led_pixel, size_t index, uint16_t hue);

// Envoie les couleurs au module : les LED changent toutes en même temps.
void led_pixel_show(led_pixel_t *led_pixel);

// Éteint toutes les LED immédiatement.
void led_pixel_clear(led_pixel_t *led_pixel);

#endif // LED_PIXEL_H
