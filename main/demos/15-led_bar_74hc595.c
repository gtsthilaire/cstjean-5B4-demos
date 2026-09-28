#include "led_bar_74hc595.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/15-led_bar_74hc595";

#define LED_BAR_74HC595_GPIO_DATA GPIO_NUM_14
#define LED_BAR_74HC595_GPIO_CLOCK GPIO_NUM_13
#define LED_BAR_74HC595_GPIO_LATCH GPIO_NUM_12

#define LED_BAR_74HC595_COUNT 8     // Le 74HC595 a 8 sorties
#define LED_BAR_74HC595_DELAY_MS 100

static void led_bar_74hc595_task(void *arg)
{
    led_bar_74hc595_t *led_bar_74hc595 = (led_bar_74hc595_t *)arg;

    while (1) {
        // 1 << i : un octet avec seulement le bit i à 1 (ex : 1 << 3 = 00001000), donc une seule LED allumée
        ESP_LOGI(TAG, "Lumière qui voyage : aller");
        for (int i = 0; i < LED_BAR_74HC595_COUNT; i++) {
            led_bar_74hc595_show(led_bar_74hc595, 1 << i);
            vTaskDelay(pdMS_TO_TICKS(LED_BAR_74HC595_DELAY_MS));
        }

        ESP_LOGI(TAG, "Lumière qui voyage : retour");
        for (int i = LED_BAR_74HC595_COUNT - 1; i >= 0; i--) {
            led_bar_74hc595_show(led_bar_74hc595, 1 << i);
            vTaskDelay(pdMS_TO_TICKS(LED_BAR_74HC595_DELAY_MS));
        }
    }
}

void start_demo_15_led_bar_74hc595(void)
{
    static led_bar_74hc595_t led_bar_74hc595;
    led_bar_74hc595 = led_bar_74hc595_init(LED_BAR_74HC595_GPIO_DATA, LED_BAR_74HC595_GPIO_CLOCK, LED_BAR_74HC595_GPIO_LATCH);

    xTaskCreate(led_bar_74hc595_task, "15-led_bar_74hc595_task", 2048, &led_bar_74hc595, 1, NULL);
}
