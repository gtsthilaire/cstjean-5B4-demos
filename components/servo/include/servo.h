#ifndef SERVO_H
#define SERVO_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define SERVO_ANGLE_MAX 180 // Angle maximal du servo (degrés)

typedef struct {
    gpio_num_t gpio;
    ledc_channel_t channel;
} servo_t;

// Configure le timer LEDC (50 Hz) et associe le GPIO au canal PWM donné.
servo_t servo_init(gpio_num_t gpio, ledc_channel_t channel);

// Positionne le servo à l'angle donné (0..SERVO_ANGLE_MAX). Les valeurs hors limites sont ramenées dans les limites.
void servo_set_angle(servo_t *servo, int angle);

#endif // SERVO_H
