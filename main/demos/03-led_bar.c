#include "led_bar.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/03-led_bar";

static const gpio_num_t LED_BAR_GPIOS[] = {
    GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18,
    GPIO_NUM_5,  GPIO_NUM_4,  GPIO_NUM_0,  GPIO_NUM_2,  GPIO_NUM_15,
};

#define LED_BAR_DELAY_MS 100

// Allume la LED, attend, puis l'éteint.
static void flash_led(led_bar_t *led_bar, int index)
{
    led_bar_set(led_bar, index, true);
    vTaskDelay(pdMS_TO_TICKS(LED_BAR_DELAY_MS));
    led_bar_set(led_bar, index, false);
}

static void led_bar_task(void *arg)
{
    led_bar_t *led_bar = (led_bar_t *)arg;
    int count = (int)led_bar->count; // int plutôt que size_t : la boucle de retour descend jusqu'à 0 sans risque

    while (1) {
        // 1. Lumière qui voyage : une seule LED allumée à la fois, aller-retour
        ESP_LOGI(TAG, "Lumière qui voyage : aller");
        for (int i = 0; i < count; i++) {
            flash_led(led_bar, i);
        }

        ESP_LOGI(TAG, "Lumière qui voyage : retour");
        for (int i = count - 1; i >= 0; i--) {
            flash_led(led_bar, i);
        }

        // 2. Bargraph : la barre se remplit puis se vide
        ESP_LOGI(TAG, "Bargraph : remplissage");
        for (int level = 0; level <= count; level++) {
            led_bar_set_level(led_bar, level);
            vTaskDelay(pdMS_TO_TICKS(LED_BAR_DELAY_MS));
        }

        ESP_LOGI(TAG, "Bargraph : vidage");
        for (int level = count; level >= 0; level--) {
            led_bar_set_level(led_bar, level);
            vTaskDelay(pdMS_TO_TICKS(LED_BAR_DELAY_MS));
        }
    }
}

void start_demo_03_led_bar(void)
{
    static led_bar_t led_bar;
    led_bar = led_bar_init(LED_BAR_GPIOS, sizeof(LED_BAR_GPIOS) / sizeof(LED_BAR_GPIOS[0]));

    xTaskCreate(led_bar_task, "03-led_bar_task", 2048, &led_bar, 1, NULL);
}
