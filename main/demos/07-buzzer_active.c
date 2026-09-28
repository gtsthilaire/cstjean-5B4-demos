#include "buzzer_active.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/07-buzzer_active";

#define BUZZER_GPIO GPIO_NUM_13

static void buzzer_active_task(void *arg)
{
    buzzer_active_t *buzzer = (buzzer_active_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "BUZZER ON");
        buzzer_active_set(buzzer, true);
        vTaskDelay(pdMS_TO_TICKS(100));

        ESP_LOGI(TAG, "BUZZER OFF");
        buzzer_active_set(buzzer, false);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_07_buzzer_active(void)
{
    static buzzer_active_t buzzer;
    buzzer = buzzer_active_init(BUZZER_GPIO);

    xTaskCreate(buzzer_active_task, "07-buzzer_active_task", 2048, &buzzer, 1, NULL);
}
