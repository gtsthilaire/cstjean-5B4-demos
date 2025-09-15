#ifndef LED_BAR_H
#define LED_BAR_H

#include "driver/gpio.h"

void start_demo_led_bar_task(gpio_num_t *leds_gpio, size_t leds_count);

#endif
