/**
 * Un joystick contient deux potentiomètres (axes X et Y) et un bouton poussoir (axe Z, quand on appuie dessus).
 * Les axes X et Y sont lus avec l'ADC, et le bouton Z comme une entrée numérique.
 * Voir components/potentiometer pour une explication plus détaillée sur l'ADC (et les broches ADC1/ADC2).
 *
 * Contrairement au potentiomètre, les deux axes partagent la même unité ADC : on ne crée l'unité qu'une seule fois,
 * puis on y configure deux canaux. Les GPIO X et Y doivent donc être sur la même unité (ex : deux broches ADC1).
 *
 * Le bouton Z réutilise le composant push_button (et profite donc de son debounce logiciel).
 *
 * Au repos, la valeur n'est pas exactement au milieu (4095 / 2) et elle tremble un peu. On mesure donc le centre
 * au démarrage, puis on donne la position en pourcentage : 0 = centre, -100 / +100 = au bout.
 * Autour du centre, une petite zone morte (±10 %) donne 0, pour que le joystick au repos affiche vraiment 0.
 *
 * Sur le module du kit, pousser vers le haut fait baisser la valeur brute de Y. On inverse donc le signe de Y
 * pour avoir la convention habituelle : droite = +, haut = +.
 *
 * Pour un résultat encore plus précis, on pourrait faire une moyenne de plusieurs lectures.
 */

#include "joystick.h"
#include "esp_log.h"
#include <stdlib.h>

static const char *TAG = "components/joystick";

#define JOYSTICK_DEAD_ZONE 10 // En dessous de ±10 %, on considère que le joystick est au centre

// Convertit une valeur brute en pourcentage (-100..+100) par rapport au centre mesuré au démarrage.
// Chaque côté a sa propre longueur : du centre à 0, et du centre à 4095.
static int to_percent(int raw, int center)
{
    int percent = 0;
    if (raw > center) {
        percent = (raw - center) * 100 / (JOYSTICK_MAX_RAW - center);
    } else if (raw < center) {
        percent = (raw - center) * 100 / center;
    }

    if (abs(percent) < JOYSTICK_DEAD_ZONE) {
        percent = 0; // zone morte
    }
    return percent;
}

joystick_t joystick_init(gpio_num_t gpio_x, gpio_num_t gpio_y, gpio_num_t gpio_z)
{
    // On retrouve l'unité (ADC1 ou ADC2) et le canal qui correspondent à chaque GPIO.
    adc_unit_t unit_x, unit_y;
    adc_channel_t channel_x, channel_y;
    adc_oneshot_io_to_channel(gpio_x, &unit_x, &channel_x);
    adc_oneshot_io_to_channel(gpio_y, &unit_y, &channel_y);

    if (unit_x != unit_y) {
        ESP_LOGE(TAG, "Les GPIO %d et %d ne sont pas sur la même unité ADC", gpio_x, gpio_y);
    }

    ESP_LOGI(TAG, "Configuration des axes X (GPIO %d, canal %d) et Y (GPIO %d, canal %d) sur l'ADC%d",
             gpio_x, channel_x, gpio_y, channel_y, unit_x + 1);

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit_x,
    };
    joystick_t joystick = { .adc_handle = NULL, .channel_x = channel_x, .channel_y = channel_y };
    adc_oneshot_new_unit(&unit_config, &joystick.adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_12, // valeurs 0..4095
        .atten = ADC_ATTEN_DB_12,    // plage ~0..3.3V
    };
    adc_oneshot_config_channel(joystick.adc_handle, channel_x, &channel_config);
    adc_oneshot_config_channel(joystick.adc_handle, channel_y, &channel_config);

    joystick.button = push_button_init(gpio_z);

    // Position au repos : sert de centre pour joystick_read_percent
    joystick_read(&joystick, &joystick.center_x, &joystick.center_y);
    ESP_LOGI(TAG, "Centre mesuré au repos : X = %d, Y = %d", joystick.center_x, joystick.center_y);

    return joystick;
}

void joystick_read(joystick_t *joystick, int *x, int *y)
{
    if (joystick == NULL || x == NULL || y == NULL) {
        return;
    }

    adc_oneshot_read(joystick->adc_handle, joystick->channel_x, x);
    adc_oneshot_read(joystick->adc_handle, joystick->channel_y, y);
}

void joystick_read_percent(joystick_t *joystick, int *x, int *y)
{
    if (joystick == NULL || x == NULL || y == NULL) {
        return;
    }

    int raw_x = 0, raw_y = 0;
    joystick_read(joystick, &raw_x, &raw_y);

    *x = to_percent(raw_x, joystick->center_x);
    *y = -to_percent(raw_y, joystick->center_y); // inversé : voir en haut
}

bool joystick_is_pressed(joystick_t *joystick)
{
    if (joystick == NULL) {
        return false;
    }

    return push_button_is_pressed(&joystick->button);
}
