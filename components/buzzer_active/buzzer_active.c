/**
 * Un buzzer actif contient son propre oscillateur : il suffit de l'alimenter (GPIO à HIGH) pour qu'il émette
 * un son, toujours à la même fréquence. Il se contrôle donc exactement comme une LED. Lui envoyer un signal PWM ne
 * changerait pas la note : c'est son oscillateur interne qui l'impose.
 *
 * Le buzzer consomme plus de courant qu'un GPIO ne peut en fournir. Dans le montage du kit, le GPIO ne l'alimente donc
 * pas directement : il commande un transistor NPN qui sert d'interrupteur. GPIO à HIGH → le transistor laisse passer
 * le courant → le buzzer sonne. GPIO à LOW → le transistor bloque → silence.
 *
 * Voir components/buzzer_passive pour un buzzer dont on choisit la fréquence (la note).
 */

#include "buzzer_active.h"
#include "esp_log.h"

static const char *TAG = "components/buzzer_active";

buzzer_active_t buzzer_active_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie", gpio);
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio, 0);

    return (buzzer_active_t){ .gpio = gpio };
}

void buzzer_active_set(buzzer_active_t *buzzer, bool on)
{
    if (buzzer == NULL) {
        return;
    }

    gpio_set_level(buzzer->gpio, on);
}
