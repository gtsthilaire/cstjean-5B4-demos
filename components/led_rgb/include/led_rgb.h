#ifndef LED_RGB_H
#define LED_RGB_H

#include "driver/gpio.h"

void start_demo_led_rgb_task(gpio_num_t gpio_r, gpio_num_t gpio_g, gpio_num_t gpio_b);

#endif