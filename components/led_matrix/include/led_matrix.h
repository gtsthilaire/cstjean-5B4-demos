#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include "driver/gpio.h"

void start_demo_led_matrix_timer(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch);

#endif
