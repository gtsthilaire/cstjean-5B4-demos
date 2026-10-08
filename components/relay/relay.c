/**
 * Un relais est un interrupteur commandé électriquement : un petit courant (venant du GPIO) actionne un
 * électroaimant qui ferme ou ouvre un contact mécanique. On l'entend d'ailleurs « claquer ».
 *
 * Il sert à allumer ou éteindre un circuit de puissance (ex : un moteur) que le GPIO ne pourrait pas alimenter
 * lui-même : un GPIO de l'ESP32 ne fournit que quelques milliampères, alors qu'un moteur en demande beaucoup plus.
 * Les deux circuits restent séparés : le moteur a sa propre alimentation.
 *
 * Limite : un relais, c'est tout ou rien. Pas de vitesse, pas de sens de rotation (voir components/dc_motor).
 *
 * Côté code, c'est exactement comme une LED (voir components/led) : un GPIO à l'état haut ou bas.
 */

#include "relay.h"
#include "esp_log.h"

static const char *TAG = "components/relay";

relay_t relay_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie", gpio);
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio, 0); // relais ouvert au démarrage

    return (relay_t){ .gpio = gpio };
}

void relay_set(relay_t *relay, bool on)
{
    if (relay == NULL) {
        return;
    }

    gpio_set_level(relay->gpio, on);
}
