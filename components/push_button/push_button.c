#include "push_button.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "components/push_button";

// Délai minimum (µs) pendant lequel la lecture doit rester stable avant
// qu'on ne mette à jour l'état "debouncé" (valeur typique pour un bouton mécanique).
#define PUSH_BUTTON_DEBOUNCE_US (50 * 1000)

push_button_t push_button_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée", gpio);
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);

    bool initial_state = gpio_get_level(gpio) == 0;

    return (push_button_t){
        .gpio = gpio,
        .debounced_state = initial_state,
        .last_raw_state = initial_state,
        .last_change_us = esp_timer_get_time(),
    };
}

bool push_button_is_pressed(push_button_t *push_button)
{
    if (push_button == NULL) {
        return false;
    }

    bool raw_state = gpio_get_level(push_button->gpio) == 0;
    int64_t now_us = esp_timer_get_time();

    if (raw_state != push_button->last_raw_state) {
        // La lecture brute vient de changer : on ne l'accepte pas tout de
        // suite, on note juste l'instant et on attend qu'elle se stabilise.
        push_button->last_raw_state = raw_state;
        push_button->last_change_us = now_us;
    } else if ((now_us - push_button->last_change_us) >= PUSH_BUTTON_DEBOUNCE_US) {
        // La lecture brute est restée stable assez longtemps : on l'accepte.
        push_button->debounced_state = raw_state;
    }

    return push_button->debounced_state;
}
