/**
 * L'ESP32 possède un capteur tactile capacitif intégré : il suffit de brancher un fil (ou une surface
 * conductrice) sur une des broches tactiles.
 *
 * Le contrôleur charge et décharge la broche en boucle et compte combien de cycles il arrive à faire pendant
 * un temps donné. Quand un doigt touche le fil, il ajoute de la capacité (le corps humain agit comme un
 * condensateur) : la charge est plus lente, donc la valeur mesurée diminue.
 *
 * Pour savoir si le capteur est touché, on mesure la valeur au repos au démarrage, puis on fixe deux seuils
 * un peu plus bas :
 *   - pour être touché, la valeur doit descendre sous le seuil bas (repos - 20 %) ;
 *   - pour être relâché, elle doit remonter au-dessus du seuil haut (repos - 10 %).
 * Entre les deux, on garde l'état précédent. Avec un seul seuil, une valeur qui tremble juste autour ferait
 * alterner Touché / Relâché plusieurs fois par seconde. Avec deux seuils, un petit tremblement ne change rien.
 *
 * Sur l'ESP32, les broches tactiles disponibles sont :
 * GPIO4 (T0), GPIO0 (T1), GPIO2 (T2), GPIO15 (T3), GPIO13 (T4), GPIO12 (T5),
 * GPIO14 (T6), GPIO27 (T7), GPIO33 (T8) et GPIO32 (T9).
 *
 * On utilise le driver touch_sens d'ESP-IDF (le driver touch_pad est déprécié).
 *
 * À noter : l'ESP32 n'a qu'un seul contrôleur tactile. On ne peut donc pas (pour l'instant) utiliser
 * deux composants touch en même temps.
 */

#include "touch.h"
#include "esp_log.h"

static const char *TAG = "components/touch";

#define TOUCH_PRESS_RATIO 0.2f     // Touché si la valeur descend de plus de 20 % sous la valeur au repos (à ajuster au besoin)
#define TOUCH_RELEASE_RATIO 0.1f   // Relâché si la valeur remonte à moins de 10 % sous la valeur au repos
#define TOUCH_INIT_SCAN_COUNT 3    // Nombre de mesures au démarrage pour stabiliser la valeur au repos

// Correspondance GPIO → canal tactile (T0 à T9) de l'ESP32. -1 = le GPIO n'est pas une broche tactile.
static int gpio_to_touch_channel(gpio_num_t gpio)
{
    switch (gpio) {
        case GPIO_NUM_4:  return 0;
        case GPIO_NUM_0:  return 1;
        case GPIO_NUM_2:  return 2;
        case GPIO_NUM_15: return 3;
        case GPIO_NUM_13: return 4;
        case GPIO_NUM_12: return 5;
        case GPIO_NUM_14: return 6;
        case GPIO_NUM_27: return 7;
        case GPIO_NUM_33: return 8;
        case GPIO_NUM_32: return 9;
        default:          return -1;
    }
}

touch_t touch_init(gpio_num_t gpio)
{
    touch_t touch = { .sensor = NULL, .channel = NULL, .touch_threshold = 0, .release_threshold = 0, .touched = false };

    int channel_id = gpio_to_touch_channel(gpio);
    if (channel_id < 0) {
        ESP_LOGE(TAG, "Le GPIO %d n'est pas une broche tactile", gpio);
        return touch;
    }

    ESP_LOGI(TAG, "Configuration du GPIO %d en capteur tactile (T%d)", gpio, channel_id);

    // Contrôleur : durée de chaque mesure (5 ms) et tensions de charge/décharge (valeurs de l'exemple d'Espressif)
    touch_sensor_sample_config_t sample_config[] = {
        TOUCH_SENSOR_V1_DEFAULT_SAMPLE_CONFIG(5.0, TOUCH_VOLT_LIM_L_0V5, TOUCH_VOLT_LIM_H_1V7),
    };
    touch_sensor_config_t sensor_config = TOUCH_SENSOR_DEFAULT_BASIC_CONFIG(1, sample_config);
    touch_sensor_new_controller(&sensor_config, &touch.sensor);

    // Canal : le seuil du matériel (abs_active_thresh) n'est pas utilisé ici, on compare nous-mêmes avec nos deux seuils
    touch_channel_config_t channel_config = {
        .abs_active_thresh = { 0 },
        .charge_speed = TOUCH_CHARGE_SPEED_7,
        .init_charge_volt = TOUCH_INIT_CHARGE_VOLT_DEFAULT,
        .group = TOUCH_CHAN_TRIG_GROUP_BOTH,
    };
    touch_sensor_new_channel(touch.sensor, channel_id, &channel_config, &touch.channel);

    // Filtre logiciel : lisse les mesures (toutes les 10 ms) pour réduire le bruit
    touch_sensor_filter_config_t filter_config = TOUCH_SENSOR_DEFAULT_FILTER_CONFIG();
    touch_sensor_config_filter(touch.sensor, &filter_config);

    touch_sensor_enable(touch.sensor);

    // Quelques mesures pour obtenir une valeur au repos stable, puis calcul des seuils
    for (int i = 0; i < TOUCH_INIT_SCAN_COUNT; i++) {
        touch_sensor_trigger_oneshot_scanning(touch.sensor, 2000);
    }
    uint32_t idle_value = touch_read(&touch);
    touch.touch_threshold = (uint32_t)(idle_value * (1.0f - TOUCH_PRESS_RATIO));
    touch.release_threshold = (uint32_t)(idle_value * (1.0f - TOUCH_RELEASE_RATIO));
    ESP_LOGI(TAG, "Valeur au repos: %lu, seuil touché: %lu, seuil relâché: %lu", (unsigned long)idle_value,
             (unsigned long)touch.touch_threshold, (unsigned long)touch.release_threshold);

    // Le contrôleur mesure maintenant en continu, en arrière-plan
    touch_sensor_start_continuous_scanning(touch.sensor);

    return touch;
}

uint32_t touch_read(touch_t *touch)
{
    if (touch == NULL || touch->channel == NULL) {
        return 0;
    }

    uint32_t value = 0;
    touch_channel_read_data(touch->channel, TOUCH_CHAN_DATA_TYPE_SMOOTH, &value);
    return value;
}

bool touch_is_touched(touch_t *touch)
{
    if (touch == NULL || touch->channel == NULL) {
        return false;
    }

    uint32_t value = touch_read(touch);

    if (!touch->touched && value < touch->touch_threshold) {
        touch->touched = true;  // descendu sous le seuil bas : touché
    } else if (touch->touched && value > touch->release_threshold) {
        touch->touched = false; // remonté au-dessus du seuil haut : relâché
    }
    // Sinon (entre les deux seuils), on garde l'état précédent

    return touch->touched;
}
