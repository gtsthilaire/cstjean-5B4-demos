#include "potentiometer.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/09-potentiometer";

#define POTENTIOMETER_GPIO GPIO_NUM_4

static void potentiometer_task(void *arg)
{
    potentiometer_t *potentiometer = (potentiometer_t *)arg;

    while (1) {
        int raw = potentiometer_read_raw(potentiometer);

        // Formule approximative du tutoriel (l'ADC de l'ESP32 n'est pas parfaitement linéaire).
        float voltage = raw / (float)POTENTIOMETER_MAX_RAW * 3.3f;

        ESP_LOGI(TAG, "ADC: raw=%4d   V≈ %.2f V", raw, voltage);

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void start_demo_09_potentiometer(void)
{
    static potentiometer_t potentiometer;
    potentiometer = potentiometer_init(POTENTIOMETER_GPIO);

    xTaskCreate(potentiometer_task, "09-potentiometer_task", 2048, &potentiometer, 1, NULL);
}
