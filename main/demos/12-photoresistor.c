#include "photoresistor.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/12-photoresistor";

#define PHOTORESISTOR_GPIO GPIO_NUM_4 // même montage que le chapitre 9

static void photoresistor_task(void *arg)
{
    photoresistor_t *photoresistor = (photoresistor_t *)arg;

    while (1) {
        int raw = photoresistor_read_raw(photoresistor);
        int percent = photoresistor_read_percent(photoresistor);

        ESP_LOGI(TAG, "ADC: raw=%4d   Lumière: %3d %%", raw, percent);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_12_photoresistor(void)
{
    static photoresistor_t photoresistor;
    photoresistor = photoresistor_init(PHOTORESISTOR_GPIO);

    xTaskCreate(photoresistor_task, "12-photoresistor_task", 2048, &photoresistor, 1, NULL);
}
