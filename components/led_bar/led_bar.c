#include "led_bar.h"
#include "esp_log.h"

static const char *TAG = "components/led_bar";

led_bar_t led_bar_init(const gpio_num_t *gpios, size_t count)
{
    led_bar_t led_bar = { .count = 0 };

    if (gpios == NULL) {
        return led_bar; // Aucune LED : les autres fonctions ne feront rien
    }

    if (count > LED_BAR_MAX_LEDS) {
        count = LED_BAR_MAX_LEDS; // Limite à LED_BAR_MAX_LEDS
    }

    for (size_t i = 0; i < count; i++) {
        ESP_LOGI(TAG, "Configuration du GPIO %d en sortie (LED %d)", gpios[i], (int)i);
        gpio_reset_pin(gpios[i]);
        gpio_set_direction(gpios[i], GPIO_MODE_OUTPUT);
        gpio_set_level(gpios[i], 0);
        led_bar.gpios[i] = gpios[i];
    }

    led_bar.count = count;
    return led_bar;
}

void led_bar_set(led_bar_t *led_bar, size_t index, bool on)
{
    if (led_bar == NULL || index >= led_bar->count) {
        return;
    }

    gpio_set_level(led_bar->gpios[index], on);
}

void led_bar_set_level(led_bar_t *led_bar, size_t level)
{
    if (led_bar == NULL) {
        return;
    }

    if (level > led_bar->count) {
        level = led_bar->count;
    }

    for (size_t i = 0; i < led_bar->count; i++) {
        led_bar_set(led_bar, i, i < level);
    }
}
