#ifndef LED_RGB_H
#define LED_RGB_H

#include <stdint.h>
#include "driver/gpio.h"
#include "led_pwm.h"

typedef struct {
    led_pwm_t r; // canal PWM de la couleur rouge
    led_pwm_t g; // canal PWM de la couleur verte
    led_pwm_t b; // canal PWM de la couleur bleue
} led_rgb_t;

// Configure chaque couleur en PWM sur son GPIO et son canal LEDC, et retourne un led_rgb_t. La LED démarre éteinte.
// Les trois canaux doivent être différents et ne pas être utilisés ailleurs en mode haute vitesse.
led_rgb_t led_rgb_init(gpio_num_t gpio_r, ledc_channel_t channel_r,
                       gpio_num_t gpio_g, ledc_channel_t channel_g,
                       gpio_num_t gpio_b, ledc_channel_t channel_b);

// Règle l'intensité de chaque couleur (0 = éteinte, 255 = intensité maximale).
void led_rgb_set(led_rgb_t *led_rgb, uint8_t r, uint8_t g, uint8_t b);

#endif // LED_RGB_H
