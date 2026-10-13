#include "ultrasonic.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/21-ultrasonic";

#define ULTRASONIC_TRIG_GPIO GPIO_NUM_13
#define ULTRASONIC_ECHO_GPIO GPIO_NUM_14

static void ultrasonic_task(void *arg)
{
    ultrasonic_t *ultrasonic = (ultrasonic_t *)arg;

    while (1) {
        float distance_cm = ultrasonic_read_cm(ultrasonic);

        if (distance_cm < 0) {
            ESP_LOGI(TAG, "Hors de portée");
        } else {
            ESP_LOGI(TAG, "Distance : %.1f cm", distance_cm);
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void start_demo_21_ultrasonic(void)
{
    static ultrasonic_t ultrasonic;
    ultrasonic = ultrasonic_init(ULTRASONIC_TRIG_GPIO, ULTRASONIC_ECHO_GPIO);

    xTaskCreate(ultrasonic_task, "21-ultrasonic_task", 4096, &ultrasonic, 1, NULL);
}
