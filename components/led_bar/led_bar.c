/**
 * Démonstration de l'utilisation d'une barre de LEDs.
 * Les LEDs s'allument et s'éteignent successivement.
 *
 * Tutoriel original : Chapitre 3 - LED bar (Freenove)
 */

#include "led_bar.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_LEDS 10

static const char *TAG = "led_bar.c";

static gpio_num_t s_led_bar_gpios[MAX_LEDS];

static size_t s_leds_count = 0;

static void led_bar_init(gpio_num_t *leds_gpio, size_t leds_count)
{
    for (size_t i = 0; i < MAX_LEDS; i++) {
        s_led_bar_gpios[i] = -1;
    }

    if (leds_count > MAX_LEDS) {
        leds_count = MAX_LEDS; // Limite à MAX_LEDS
    }

    s_leds_count = leds_count;

    for (size_t i = 0; i < s_leds_count; i++) {
        s_led_bar_gpios[i] = leds_gpio[i];
        gpio_reset_pin(s_led_bar_gpios[i]);
        gpio_set_direction(s_led_bar_gpios[i], GPIO_MODE_OUTPUT);
        gpio_set_level(s_led_bar_gpios[i], 0);
    }
}

static void led_bar_task(void *arg)
{
    while (1) {
        for (int i = 0; i < s_leds_count; i++) {
            ESP_LOGD(TAG, "Led %d ON", s_led_bar_gpios[i]);
            gpio_set_level(s_led_bar_gpios[i], 1);
            vTaskDelay(pdMS_TO_TICKS(500));
        }

        for (int i = s_leds_count - 1; i >= 0; i--) {
            ESP_LOGD(TAG, "Led %d OFF", s_led_bar_gpios[i]);
            gpio_set_level(s_led_bar_gpios[i], 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

void start_demo_led_bar_task(gpio_num_t *leds_gpio, size_t leds_count)
{
    led_bar_init(leds_gpio, leds_count);

    xTaskCreate(led_bar_task, "led_bar_task", 2048, NULL, 5, NULL);
}