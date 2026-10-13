#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <stdbool.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio;
} motion_sensor_t;

// Configure le GPIO en entrée et retourne un motion_sensor_t.
motion_sensor_t motion_sensor_init(gpio_num_t gpio);

// Retourne true si le capteur détecte un mouvement, false sinon.
bool motion_sensor_is_detected(motion_sensor_t *motion_sensor);

#endif // MOTION_SENSOR_H
