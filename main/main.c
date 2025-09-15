#include "led.h"
#include "push_button.h"
#include "led_bar.h"
#include "led_pwm.h"
#include "led_rgb.h"
#include "led_pixel.h"
#include "buzzer_active.h"
#include "buzzer_passive.h"
#include "potentiometer.h"
#include "touch.h"

void app_main(void)
{
    // components/led/led.c
    // start_demo_led_task(GPIO_NUM_2);

    // components/push_button/push_button.c
    // start_demo_push_button_task(GPIO_NUM_2);

    // components/led_bar/led_bar.c
    // gpio_num_t led_pins[] =  {GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_0, GPIO_NUM_2, GPIO_NUM_15};
    // start_demo_led_bar_task(led_pins, sizeof(led_pins) / sizeof(led_pins[0]));

    // components/led_pwm/led_pwm.c
    // start_demo_led_pwm_task(GPIO_NUM_2);

    // components/led_rgb/led_rgb.c
    // start_demo_led_rgb_task(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4);

    // components/led_pixel/led_pixel.c
    // start_demo_led_pixel_task(GPIO_NUM_2);

    // components/buzzer_active/buzzer_active.c
    // start_demo_buzzer_active_task(GPIO_NUM_13);

    // components/buzzer_passive/buzzer_passive.c
    // start_demo_buzzer_passive_task(GPIO_NUM_13);

    // components/potentiometer/potentiometer.c
    // start_demo_potentiometer_task(ADC_CHANNEL_6); // GPIO34

    // components/touch/touch.c
    // start_demo_touch_task(TOUCH_PAD_NUM0); // GPIO4
}