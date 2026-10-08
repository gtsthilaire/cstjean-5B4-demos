#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio_trig;
    gpio_num_t gpio_echo;
} ultrasonic_t;

// Configure Trig en sortie et Echo en entrée, puis retourne un ultrasonic_t.
ultrasonic_t ultrasonic_init(gpio_num_t gpio_trig, gpio_num_t gpio_echo);

// Mesure la distance de l'obstacle (en cm). Retourne -1 si aucun écho ne revient (obstacle trop loin ou absent).
float ultrasonic_read_cm(ultrasonic_t *ultrasonic);

#endif // ULTRASONIC_H
