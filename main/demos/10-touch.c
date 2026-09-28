#include "touch.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/10-touch";

#define TOUCH_GPIO GPIO_NUM_4 // T0

#define CHECK_PERIOD_MS 50    // On vérifie l'état souvent, pour réagir vite
#define PRINT_PERIOD_MS 500   // Mais on n'affiche la valeur que de temps en temps, pour garder la console lisible

static void touch_task(void *arg)
{
    touch_t *touch = (touch_t *)arg;
    bool was_touched = false;
    int elapsed_ms = 0;

    while (1) {
        bool is_touched = touch_is_touched(touch);

        if (is_touched != was_touched) {
            ESP_LOGI(TAG, "%s", is_touched ? "Touché" : "Relâché");
            was_touched = is_touched;
        }

        elapsed_ms += CHECK_PERIOD_MS;
        if (elapsed_ms >= PRINT_PERIOD_MS) {
            ESP_LOGI(TAG, "Valeur: %5lu (seuil touché: %lu)", (unsigned long)touch_read(touch),
                     (unsigned long)touch->touch_threshold);
            elapsed_ms = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(CHECK_PERIOD_MS));
    }
}

void start_demo_10_touch(void)
{
    static touch_t touch;
    touch = touch_init(TOUCH_GPIO);

    xTaskCreate(touch_task, "10-touch_task", 2048, &touch, 1, NULL);
}
