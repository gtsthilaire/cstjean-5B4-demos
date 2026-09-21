#include "push_button.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/02-push_button";

#define PUSH_BUTTON_GPIO GPIO_NUM_13

static void push_button_task(void *arg)
{
    push_button_t *push_button = (push_button_t *)arg;
    bool was_pressed = false;

    while (1) {
        bool is_pressed = push_button_is_pressed(push_button);

        if (is_pressed != was_pressed) {
            if (is_pressed) {
                ESP_LOGI(TAG, "Bouton pressé");
            } else {
                ESP_LOGI(TAG, "Bouton relâché");
            }
            was_pressed = is_pressed;
        }

        // Rapide pour laisser le debounce logiciel (voir push_button.c) faire son travail.
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void start_demo_02_push_button(void)
{
    static push_button_t push_button;
    push_button = push_button_init(PUSH_BUTTON_GPIO);

    xTaskCreate(push_button_task, "02-push_button_task", 2048, &push_button, 1, NULL);
}
