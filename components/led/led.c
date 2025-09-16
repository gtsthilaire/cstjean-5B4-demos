/**
 * Démonstration de l'utilisation d'une LED simple.
 * La LED s'allume et s'éteint toutes les X millisecondes.
 *
 * Tutoriel original : Chapitre 1 - LED (Freenove)
 */

#include "led.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "led.c";

static gpio_num_t s_led_gpio = -1;

static void led_init(gpio_num_t gpio)
{
    s_led_gpio = gpio;
    gpio_reset_pin(s_led_gpio);
    gpio_set_direction(s_led_gpio, GPIO_MODE_OUTPUT);
}

static void led_task(void *arg)
{
    while (1) {
        ESP_LOGI(TAG, "Led ON");
        gpio_set_level(s_led_gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(500));

        ESP_LOGI(TAG, "Led OFF");
        gpio_set_level(s_led_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_led_task(gpio_num_t gpio)
{
    led_init(gpio);

    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
}