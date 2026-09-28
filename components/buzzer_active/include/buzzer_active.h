#ifndef BUZZER_ACTIVE_H
#define BUZZER_ACTIVE_H

#include <stdbool.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio;
} buzzer_active_t;

// Configure le GPIO en sortie et retourne un buzzer_active_t. Le buzzer démarre silencieux.
buzzer_active_t buzzer_active_init(gpio_num_t gpio);

// Fait sonner (true) ou arrête (false) le buzzer.
void buzzer_active_set(buzzer_active_t *buzzer, bool on);

#endif // BUZZER_ACTIVE_H
