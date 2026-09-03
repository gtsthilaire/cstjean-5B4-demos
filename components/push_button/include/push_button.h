#ifndef PUSH_BUTTON_H
#define PUSH_BUTTON_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio;
    bool debounced_state;   // dernier état stable retourné par push_button_is_pressed
    bool last_raw_state;    // dernière lecture brute du GPIO
    int64_t last_change_us; // horodatage (esp_timer_get_time) du dernier changement de lecture brute
} push_button_t;

// Configure le GPIO en entrée et retourne un push_button_t.
push_button_t push_button_init(gpio_num_t gpio);

// Retourne true si le bouton est pressé, false sinon (avec debounce logiciel).
bool push_button_is_pressed(push_button_t *push_button);

#endif // PUSH_BUTTON_H
