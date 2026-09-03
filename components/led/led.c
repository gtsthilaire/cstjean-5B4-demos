#include "led.h"
#include "esp_log.h"

static const char *TAG = "components/led";

led_t led_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie", gpio);
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);

    return (led_t){ .gpio = gpio, .is_on = false, .blink_timer = NULL };
}

void led_set(led_t *led, bool on)
{
    if (led == NULL) {
        return;
    }

    ESP_LOGI(TAG, "LED (GPIO %d): %s", led->gpio, on ? "ON" : "OFF");
    led->is_on = on;
    gpio_set_level(led->gpio, on);
}

static void blink_timer_callback(void *arg)
{
    led_t *led = (led_t *)arg;
    led_set(led, !led->is_on);
}

void led_start_blinking(led_t *led, uint32_t interval_ms)
{
    if (led == NULL || interval_ms == 0) {
        return;
    }

    if (led->blink_timer == NULL) {
        const esp_timer_create_args_t timer_args = {
            .callback = &blink_timer_callback,
            .arg = led,
            .name = "led_blink",
        };
        esp_timer_create(&timer_args, &led->blink_timer);
    }

    esp_timer_stop(led->blink_timer); // Pour réinitialiser le timer s'il était déjà en cours

    ESP_LOGI(TAG, "LED (GPIO %d): clignotement démarré", led->gpio);

    led_set(led, true); // Pour allumer la LED immédiatement
    esp_timer_start_periodic(led->blink_timer, (uint64_t)interval_ms * 1000);
}

void led_stop_blinking(led_t *led)
{
    if (led == NULL || led->blink_timer == NULL) {
        return;
    }

    esp_timer_stop(led->blink_timer);
    ESP_LOGI(TAG, "LED (GPIO %d): clignotement arrêté", led->gpio);
    led_set(led, false);
}
