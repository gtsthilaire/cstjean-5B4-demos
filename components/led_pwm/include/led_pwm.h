#ifndef LED_PWM_H
#define LED_PWM_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

#define LED_PWM_DUTY_BITS 8                                // Résolution du rapport cyclique (nombre de bits)
#define LED_PWM_DUTY_MAX ((1 << LED_PWM_DUTY_BITS) - 1)    // Valeur maximale du rapport cyclique (255 pour 8 bits)

typedef struct {
    gpio_num_t gpio;
    ledc_channel_t channel;
} led_pwm_t;

// Configure le timer LEDC et associe le GPIO au canal PWM donné. La LED démarre éteinte.
led_pwm_t led_pwm_init(gpio_num_t gpio, ledc_channel_t channel);

// Règle le rapport cyclique (0..LED_PWM_DUTY_MAX) : 0 = éteinte, LED_PWM_DUTY_MAX = luminosité maximale.
void led_pwm_set_duty(led_pwm_t *led_pwm, uint32_t duty);

#endif // LED_PWM_H
