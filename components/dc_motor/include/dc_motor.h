#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define DC_MOTOR_SPEED_MAX 100 // Vitesse maximale (en %)

typedef struct {
    gpio_num_t gpio_in1;
    gpio_num_t gpio_in2;
    ledc_channel_t channel;
} dc_motor_t;

// Configure les 2 GPIO de sens (IN1, IN2) en sortie et le GPIO enable en sortie PWM. Le moteur démarre arrêté.
dc_motor_t dc_motor_init(gpio_num_t gpio_in1, gpio_num_t gpio_in2, gpio_num_t gpio_enable, ledc_channel_t channel);

// Règle la vitesse (-DC_MOTOR_SPEED_MAX..DC_MOTOR_SPEED_MAX) : positive = un sens, négative = l'autre sens, 0 = arrêt.
void dc_motor_set_speed(dc_motor_t *dc_motor, int speed);

#endif // DC_MOTOR_H
