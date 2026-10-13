/**
 * Le HC-SR501 détecte la chaleur (infrarouge) émise par les humains et les animaux qui bougent.
 * Sa sortie est simple : état haut (3.3V) quand il détecte un mouvement, état bas sinon.
 * Côté code, c'est donc une simple entrée numérique, comme un bouton (voir components/push_button).
 *
 * Mais le capteur a son propre comportement, réglé directement sur le module.
 * Retourne le module (le dôme blanc en dessous) : on y voit deux petites vis orange qu'on tourne
 * avec un petit tournevis (ce sont des potentiomètres) et un petit capuchon noir sur 3 broches (le cavalier).
 *   - Une des vis règle le délai : la sortie reste haute quelques secondes (jusqu'à quelques minutes)
 *     après le mouvement. Tourner vers la gauche = délai plus court.
 *   - L'autre vis règle la sensibilité : la distance de détection.
 *   - Le cavalier (jumper) : selon sa position, soit la sortie retombe après le délai même si on bouge encore
 *     (déclenchement unique), soit chaque nouveau mouvement relance le délai (déclenchement répété).
 *   - Au démarrage, le capteur a besoin d'environ 1 minute pour se stabiliser : il peut donner de fausses
 *     détections pendant ce temps.
 */

#include "motion_sensor.h"
#include "esp_log.h"

static const char *TAG = "components/motion_sensor";

motion_sensor_t motion_sensor_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée", gpio);
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio, GPIO_PULLDOWN_ONLY); // état bas si le capteur est débranché

    return (motion_sensor_t){ .gpio = gpio };
}

bool motion_sensor_is_detected(motion_sensor_t *motion_sensor)
{
    if (motion_sensor == NULL) {
        return false;
    }

    return gpio_get_level(motion_sensor->gpio) == 1;
}
