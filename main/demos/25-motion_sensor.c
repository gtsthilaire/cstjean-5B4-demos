#include "motion_sensor.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/25-motion_sensor";

#define MOTION_SENSOR_GPIO GPIO_NUM_13

static void motion_sensor_task(void *arg)
{
    motion_sensor_t *motion_sensor = (motion_sensor_t *)arg;
    bool was_detected = false;

    while (1) {
        bool detected = motion_sensor_is_detected(motion_sensor);

        // On affiche seulement quand l'état change
        if (detected && !was_detected) {
            ESP_LOGI(TAG, "Mouvement détecté");
        } else if (!detected && was_detected) {
            ESP_LOGI(TAG, "Plus de mouvement");
        }
        was_detected = detected;

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void start_demo_25_motion_sensor(void)
{
    static motion_sensor_t motion_sensor;
    motion_sensor = motion_sensor_init(MOTION_SENSOR_GPIO);

    xTaskCreate(motion_sensor_task, "25-motion_sensor_task", 2048, &motion_sensor, 1, NULL);
}
