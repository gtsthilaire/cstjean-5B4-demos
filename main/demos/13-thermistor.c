#include "thermistor.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/13-thermistor";

#define THERMISTOR_GPIO GPIO_NUM_4 // même montage que les chapitres 9 et 12

static void thermistor_task(void *arg)
{
    thermistor_t *thermistor = (thermistor_t *)arg;

    while (1) {
        int raw = thermistor_read_raw(thermistor);
        float celsius = thermistor_read_celsius(thermistor);

        ESP_LOGI(TAG, "ADC: raw=%4d   Temp: %.1f °C", raw, celsius);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_13_thermistor(void)
{
    static thermistor_t thermistor;
    thermistor = thermistor_init(THERMISTOR_GPIO);

    xTaskCreate(thermistor_task, "13-thermistor_task", 2048, &thermistor, 1, NULL);
}
