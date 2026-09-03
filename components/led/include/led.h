#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_timer.h"
#include <stdint.h>

typedef struct {
    gpio_num_t gpio;
    bool is_on;
    esp_timer_handle_t blink_timer;
} led_t;

// Configure le GPIO en sortie et retourne un led_t.
led_t led_init(gpio_num_t gpio);

// Allume (true) ou éteint (false) la LED immédiatement.
void led_set(led_t *led, bool on);

// Démarre un clignotement périodique (via esp_timer) à l'intervalle donné.
void led_start_blinking(led_t *led, uint32_t interval_ms);

// Arrête le clignotement démarré par led_start_blinking et éteint la LED.
void led_stop_blinking(led_t *led);

#endif // LED_H
