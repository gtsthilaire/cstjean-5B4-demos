#include "led.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/01-led";

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

void led_start_demo(void)
{
    // "static" est essentiel ici : la tâche créée ci-dessous garde un pointeur
    // vers "led" bien après le retour de cette fonction. Sans "static", "led"
    // serait détruite avec la pile de led_start_demo().
    static led_t led;
    led = led_init(GPIO_NUM_2);

    // Voir aussi led_start_blinking plutôt que de créer une tâche pour faire clignoter la LED
    xTaskCreate(led_task, "01-led_task", 2048, &led, 1, NULL);
}
