/**
 * Démonstration de l'utilisation d'un buzzer actif.
 * Le buzzer émet un son toutes les X millisecondes.
 *
 * Tutoriel original : Chapitre 7 - Buzzer (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de gestion de bouton pour activer/désactiver le buzzer.
 */

#include "buzzer_active.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "buzzer_active.c";

static gpio_num_t s_buzzer_gpio = -1;

static void buzzer_active_init(gpio_num_t buzzer_gpio)
{
    s_buzzer_gpio = buzzer_gpio;
    gpio_reset_pin(s_buzzer_gpio);
    gpio_set_direction(s_buzzer_gpio, GPIO_MODE_OUTPUT);
}

static void buzzer_active_task(void *arg)
{
    while (1) {
        ESP_LOGD(TAG, "Buzzer ON");
        gpio_set_level(s_buzzer_gpio, 1);
        vTaskDelay(pdMS_TO_TICKS(100));

        ESP_LOGD(TAG, "Buzzer OFF");
        gpio_set_level(s_buzzer_gpio, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_buzzer_active_task(gpio_num_t buzzer_gpio)
{
    buzzer_active_init(buzzer_gpio);
    
    xTaskCreate(buzzer_active_task, "buzzer_active_task", 2048, NULL, 5, NULL);
}