/**
 * Une LED RGB contient trois LED (rouge, verte et bleue) dans le même boîtier. En faisant varier l'intensité
 * de chacune avec du PWM, on peut mélanger les couleurs (ex : rouge + vert = jaune, les trois = blanc).
 *
 * Ce composant réutilise le composant led_pwm : chaque couleur est simplement une LED PWM sur son propre canal.
 * Voir components/led_pwm pour une explication plus détaillée sur le PWM.
 *
 * La LED RGB du kit est à anode commune : les trois LED partagent la même anode (+), reliée au 3.3V, et chaque
 * cathode (-) est reliée à un GPIO (via une résistance). Une LED ne s'allume que si le courant la traverse de
 * l'anode vers la cathode, donc s'il y a une différence de tension entre les deux :
 *   - GPIO à HIGH (3.3V) : 3.3V des deux côtés, aucun courant, la couleur est éteinte.
 *   - GPIO à LOW  (0V)   : le courant circule du 3.3V vers le GPIO, la couleur s'allume.
 * C'est l'inverse d'une LED ordinaire (le GPIO absorbe le courant au lieu de le fournir).
 *
 * Conséquence pour le PWM : le rapport cyclique (duty) est le pourcentage du temps où le GPIO est à HIGH. Ici, la
 * LED est allumée pendant le LOW, donc le temps allumé = 100 % - duty. Sans correction, l'échelle est à l'envers
 * (0 = intensité maximale, 255 = éteinte).
 *
 * On demande donc au périphérique LEDC d'inverser la sortie (inverted = true) : on garde 0 = éteinte,
 * 255 = intensité maximale. On pourrait aussi inverser en logiciel (envoyer 255 - valeur), mais l'inversion
 * matérielle s'applique dès la configuration du canal : sinon, le duty de 0 donné à l'initialisation mettrait le
 * GPIO à LOW en permanence et la LED s'allumerait en blanc au démarrage.
 */

#include "led_rgb.h"
#include "esp_log.h"

static const char *TAG = "components/led_rgb";

led_rgb_t led_rgb_init(gpio_num_t gpio_r, ledc_channel_t channel_r,
                       gpio_num_t gpio_g, ledc_channel_t channel_g,
                       gpio_num_t gpio_b, ledc_channel_t channel_b)
{
    ESP_LOGI(TAG, "Configuration de la LED RGB (R: GPIO %d canal %d, G: GPIO %d canal %d, B: GPIO %d canal %d)",
             gpio_r, channel_r, gpio_g, channel_g, gpio_b, channel_b);

    // Sortie inversée, car la LED est active à LOW (anode commune)
    led_rgb_t led_rgb = {
        .r = led_pwm_init(gpio_r, channel_r, true),
        .g = led_pwm_init(gpio_g, channel_g, true),
        .b = led_pwm_init(gpio_b, channel_b, true),
    };

    return led_rgb;
}

void led_rgb_set(led_rgb_t *led_rgb, uint8_t r, uint8_t g, uint8_t b)
{
    if (led_rgb == NULL) {
        return;
    }

    led_pwm_set_duty(&led_rgb->r, r);
    led_pwm_set_duty(&led_rgb->g, g);
    led_pwm_set_duty(&led_rgb->b, b);
}
