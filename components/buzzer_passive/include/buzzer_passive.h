#ifndef BUZZER_PASSIVE_H
#define BUZZER_PASSIVE_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/ledc.h"

typedef struct {
    gpio_num_t gpio;
    ledc_channel_t channel;
} buzzer_passive_t;

// Configure le timer LEDC et associe le GPIO au canal PWM donné. Le buzzer démarre silencieux.
buzzer_passive_t buzzer_passive_init(gpio_num_t gpio, ledc_channel_t channel);

// Joue un son continu à la fréquence donnée (en Hz, de ~76 Hz à ~78 kHz). 0 Hz arrête le son (silence).
void buzzer_passive_tone(buzzer_passive_t *buzzer, uint32_t freq_hz);

// Arrête le son.
void buzzer_passive_stop(buzzer_passive_t *buzzer);

#endif // BUZZER_PASSIVE_H
