#include "led_pwm.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/04-led_pwm";

#define LED_PWM_GPIO GPIO_NUM_2
#define LED_PWM_CHANNEL LEDC_CHANNEL_0

static void led_pwm_task(void *arg)
{
    led_pwm_t *led_pwm = (led_pwm_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "LED FADE IN");
        for (int duty = 0; duty <= LED_PWM_DUTY_MAX; duty++) {
            led_pwm_set_duty(led_pwm, duty);
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        ESP_LOGI(TAG, "LED FADE OUT");
        for (int duty = LED_PWM_DUTY_MAX; duty >= 0; duty--) {
            led_pwm_set_duty(led_pwm, duty);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void start_demo_04_led_pwm(void)
{
    static led_pwm_t led_pwm;
    led_pwm = led_pwm_init(LED_PWM_GPIO, LED_PWM_CHANNEL);

    xTaskCreate(led_pwm_task, "04-led_pwm_task", 2048, &led_pwm, 1, NULL);
}
