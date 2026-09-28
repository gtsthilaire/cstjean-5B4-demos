/**
 * Une photorésistance est un composant dont la résistance diminue quand la lumière augmente.
 *
 * Montée en diviseur de tension avec une résistance fixe, elle produit une tension qui varie avec la luminosité.
 * On lit cette tension avec l'ADC, exactement comme pour le potentiomètre.
 *
 * Dans le montage du kit, la photorésistance est en série avec une résistance fixe de 10 kΩ.
 * Les 3.3V se partagent entre les deux résistances : chacune prend une part proportionnelle à sa taille.
 * La GPIO lit la part de la photorésistance :
 *   - dans le noir, elle est grande → elle prend presque tout → tension proche de 3.3V (valeur haute) ;
 *   - en pleine lumière, elle est petite → elle prend presque rien → tension proche de 0V (valeur basse).
 *
 * La valeur brute est donc à l'envers. Pour le pourcentage, on l'inverse (100 % - ...) :
 * 0 % = noir, 100 % = très éclairé.
 * Voir components/potentiometer pour une explication plus détaillée sur l'ADC (et les broches ADC1/ADC2).
 *
 * À noter : chaque photoresistor_init crée sa propre unité ADC. On ne peut donc pas (pour l'instant) utiliser
 * deux composants sur la même unité ADC en même temps.
 */

#include "photoresistor.h"
#include "esp_log.h"

static const char *TAG = "components/photoresistor";

photoresistor_t photoresistor_init(gpio_num_t gpio)
{
    // On retrouve l'unité (ADC1 ou ADC2) et le canal qui correspondent au GPIO.
    adc_unit_t unit;
    adc_channel_t channel;
    adc_oneshot_io_to_channel(gpio, &unit, &channel);

    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée analogique (ADC%d, canal %d)", gpio, unit + 1, channel);

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit,
    };
    photoresistor_t photoresistor = { .adc_handle = NULL, .channel = channel };
    adc_oneshot_new_unit(&unit_config, &photoresistor.adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_12, // valeurs 0..4095
        .atten = ADC_ATTEN_DB_12,    // plage ~0..3.3V
    };
    adc_oneshot_config_channel(photoresistor.adc_handle, channel, &channel_config);

    return photoresistor;
}

int photoresistor_read_raw(photoresistor_t *photoresistor)
{
    if (photoresistor == NULL) {
        return 0;
    }

    int raw = 0;
    adc_oneshot_read(photoresistor->adc_handle, photoresistor->channel, &raw);
    return raw;
}

int photoresistor_read_percent(photoresistor_t *photoresistor)
{
    return 100 - photoresistor_read_raw(photoresistor) * 100 / PHOTORESISTOR_MAX_RAW; // inversé : voir en haut
}
