/**
 * Démonstration de l'utilisation d'un push button simple.
 * Le bouton est surveillé et son état est affiché dans la console.
 *
 * Tutoriel original : Chapitre 2 - Button & LED (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de gestion d'une LED.
 * J'ai également ajouté un debounce logiciel.
 */

#include "push_button.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "push_button.c";

static gpio_num_t s_push_button_gpio = -1;

// Pour debounce
static int s_last_raw_level = 1;
static int s_stable_level = 1;
static TickType_t s_last_debounce_time = 0;

static void push_button_init(gpio_num_t gpio)
{
    s_push_button_gpio = gpio;
    gpio_reset_pin(s_push_button_gpio);
    gpio_set_direction(s_push_button_gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(s_push_button_gpio, GPIO_PULLUP_ONLY);
}

static void push_button_task(void *arg)
{
    while (1) {
        int raw_level = gpio_get_level(s_push_button_gpio);

        if (raw_level != s_last_raw_level) {
            s_last_debounce_time = xTaskGetTickCount();
            s_last_raw_level = raw_level;
        }

        // Si le niveau est stable depuis plus de 50 ms
        if ((xTaskGetTickCount() - s_last_debounce_time) > pdMS_TO_TICKS(50)) {
            if (s_stable_level != raw_level) {
                s_stable_level = raw_level;
                
                if (s_stable_level == 0) {
                    ESP_LOGD(TAG, "Push button PRESSED");
                } else {
                    ESP_LOGD(TAG, "Push button RELEASED");
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void start_demo_push_button_task(gpio_num_t gpio)
{
    push_button_init(gpio);
    
    xTaskCreate(push_button_task, "push_button_task", 2048, NULL, 5, NULL);
}