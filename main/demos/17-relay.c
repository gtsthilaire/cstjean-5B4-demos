#include "relay.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/17-relay";

#define RELAY_GPIO GPIO_NUM_13

static void relay_task(void *arg)
{
    relay_t *relay = (relay_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "Moteur en marche");
        relay_set(relay, true);
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Moteur arrêté");
        relay_set(relay, false);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void start_demo_17_relay(void)
{
    static relay_t relay;
    relay = relay_init(RELAY_GPIO);

    xTaskCreate(relay_task, "17-relay_task", 4096, &relay, 1, NULL);
}
