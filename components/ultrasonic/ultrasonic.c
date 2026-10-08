/**
 * Le HC-SR04 mesure une distance avec le son, comme une chauve-souris :
 *   1. On envoie une courte impulsion sur Trig : le module émet un « clic » d'ultrasons (inaudible pour nous).
 *   2. Le module met Echo à l'état haut, et le remet à l'état bas quand l'écho revient.
 *   3. La durée de l'impulsion sur Echo = le temps de l'aller-retour du son jusqu'à l'obstacle.
 *
 * Le son parcourt environ 1 cm toutes les 29 µs. Comme il fait l'aller ET le retour, on divise par 2.
 */

#include "ultrasonic.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

static const char *TAG = "components/ultrasonic";

#define ULTRASONIC_TIMEOUT_US 25000 // Au-delà (environ 4 m), on considère qu'aucun écho n'est revenu
#define ULTRASONIC_US_PER_CM 58.0f  // Aller-retour : environ 58 µs par cm de distance

// Reste dans la boucle tant que Echo n'est pas au niveau demandé (0 = bas, 1 = haut).
// Retourne false si on attend trop longtemps (aucun écho).
static bool wait_for_level(gpio_num_t gpio, int level)
{
    int64_t start_us = esp_timer_get_time();
    while (gpio_get_level(gpio) != level) {
        if (esp_timer_get_time() - start_us > ULTRASONIC_TIMEOUT_US) {
            return false;
        }
    }
    return true;
}

ultrasonic_t ultrasonic_init(gpio_num_t gpio_trig, gpio_num_t gpio_echo)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d (Trig) en sortie et du GPIO %d (Echo) en entrée", gpio_trig, gpio_echo);

    gpio_reset_pin(gpio_trig);
    gpio_set_direction(gpio_trig, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio_trig, 0);

    gpio_reset_pin(gpio_echo);
    gpio_set_direction(gpio_echo, GPIO_MODE_INPUT);

    return (ultrasonic_t){ .gpio_trig = gpio_trig, .gpio_echo = gpio_echo };
}

float ultrasonic_read_cm(ultrasonic_t *ultrasonic)
{
    if (ultrasonic == NULL) {
        return -1;
    }

    // 1. Impulsion de 10 µs sur Trig pour déclencher une mesure
    gpio_set_level(ultrasonic->gpio_trig, 1);
    esp_rom_delay_us(10);
    gpio_set_level(ultrasonic->gpio_trig, 0);

    // Ce qu'on voit sur les broches pendant une mesure :
    //
    //   Trig : __|‾|______________________________
    //   Echo : ________|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|_____
    //                  ^                     ^
    //            le son part         l'écho revient
    //                  |<-- durée mesurée -->|
    //
    // On mesure la largeur de l'impulsion sur Echo : il faut repérer son début (0 → 1) et sa fin (1 → 0).

    // 2. Début de l'impulsion : on attend que Echo passe de 0 à 1, puis on démarre le chrono
    if (!wait_for_level(ultrasonic->gpio_echo, 1)) {
        return -1;
    }
    int64_t start_us = esp_timer_get_time();

    // 3. Fin de l'impulsion : on attend que Echo repasse de 1 à 0, puis on arrête le chrono
    if (!wait_for_level(ultrasonic->gpio_echo, 0)) {
        return -1;
    }
    int64_t echo_us = esp_timer_get_time() - start_us;

    // Durée de l'aller-retour → distance
    return echo_us / ULTRASONIC_US_PER_CM;
}
