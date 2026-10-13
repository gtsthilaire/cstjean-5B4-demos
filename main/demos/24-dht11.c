#include "dht11.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/24-dht11";

#define DHT11_GPIO GPIO_NUM_13

static void dht11_task(void *arg)
{
    dht11_t *dht11 = (dht11_t *)arg;

    while (1) {
        int temperature, humidity;

        if (dht11_read(dht11, &temperature, &humidity)) {
            ESP_LOGI(TAG, "Température : %d °C   Humidité : %d %%", temperature, humidity);
        } else {
            ESP_LOGI(TAG, "Lecture échouée");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // le DHT11 a besoin d'au moins 1 s entre deux lectures
    }
}

void start_demo_24_dht11(void)
{
    static dht11_t dht11;
    dht11 = dht11_init(DHT11_GPIO);

    xTaskCreate(dht11_task, "24-dht11_task", 4096, &dht11, 1, NULL);
}
