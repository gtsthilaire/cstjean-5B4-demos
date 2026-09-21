#include "led.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/01-led";

#define LED_GPIO GPIO_NUM_2

static void led_task(void *arg)
{
    led_t *led = (led_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "LED ON");
        led_set(led, true);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "LED OFF");
        led_set(led, false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_01_led(void)
{
    static led_t led;
    led = led_init(LED_GPIO);

    // Voir aussi led_start_blinking plutôt que de créer une tâche pour faire clignoter la LED
    xTaskCreate(led_task, "01-led_task", 2048, &led, 1, NULL);
}
