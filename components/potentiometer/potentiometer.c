/**
 * Le potentiomètre permet de varier une tension analogique en fonction de sa position.
 *
 * Le ADC (Analog to Digital Converter) est utilisé pour lire cette tension. Il la convertit en une valeur
 * numérique (ici sur 12 bits, soit de 0 à 4095).
 *
 * Sur ESP32, les broches ADC1 disponibles sont :
 * GPIO36 (ADC1_CH0), GPIO39 (ADC1_CH3), GPIO32 (ADC1_CH4), GPIO33 (ADC1_CH5), GPIO34 (ADC1_CH6), GPIO35 (ADC1_CH7).
 *
 * Il y a aussi des broches ADC2 (ex : GPIO4 = ADC2_CH0), mais l'ADC2 est partagé avec le Wi-Fi : il ne peut pas
 * être utilisé pendant que le Wi-Fi est actif. Si on prévoit utiliser le Wi-Fi, il est préférable d'utiliser l'ADC1.
 *
 * Il suffit de donner le GPIO à potentiometer_init : l'unité (ADC1 ou ADC2) et le canal sont déduits automatiquement.
 *
 * À noter : chaque potentiometer_init crée sa propre unité ADC. On ne peut donc pas (pour l'instant) utiliser
 * deux composants sur la même unité ADC en même temps.
 */

#include "potentiometer.h"
#include "esp_log.h"

static const char *TAG = "components/potentiometer";

potentiometer_t potentiometer_init(gpio_num_t gpio)
{
    // On retrouve l'unité (ADC1 ou ADC2) et le canal qui correspondent au GPIO.
    adc_unit_t unit;
    adc_channel_t channel;
    adc_oneshot_io_to_channel(gpio, &unit, &channel);

    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée analogique (ADC%d, canal %d)", gpio, unit + 1, channel);

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit,
    };
    potentiometer_t potentiometer = { .adc_handle = NULL, .channel = channel };
    adc_oneshot_new_unit(&unit_config, &potentiometer.adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_12, // valeurs 0..4095
        .atten = ADC_ATTEN_DB_12,    // plage ~0..3.3V
    };
    adc_oneshot_config_channel(potentiometer.adc_handle, channel, &channel_config);

    return potentiometer;
}

int potentiometer_read_raw(potentiometer_t *potentiometer)
{
    if (potentiometer == NULL) {
        return 0;
    }

    int raw = 0;
    adc_oneshot_read(potentiometer->adc_handle, potentiometer->channel, &raw);
    return raw;
}
