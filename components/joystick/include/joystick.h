#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"

void start_demo_joystick_task(adc_channel_t channel_x, adc_channel_t channel_y, gpio_num_t gpio_z);

#endif
