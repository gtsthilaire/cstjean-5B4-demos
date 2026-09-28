/**
 * Une thermistance est un composant dont la résistance varie avec la température. Celle du kit est de type
 * NTC (Negative Temperature Coefficient) : sa résistance diminue quand la température augmente.
 *
 * Montée en diviseur de tension avec une résistance fixe de 10 kΩ, elle produit une tension qu'on lit avec l'ADC,
 * exactement comme pour le potentiomètre et la photorésistance.
 * Voir components/potentiometer pour une explication plus détaillée sur l'ADC (et les broches ADC1/ADC2).
 *
 * Comme pour la photorésistance, les 3.3V se partagent entre les deux résistances, et la GPIO lit la part de la
 * thermistance. Plus il fait chaud, plus sa résistance baisse, plus la valeur lue baisse.
 *
 * L'ADC de l'ESP32 n'est pas parfaitement précis : il lit un peu plus bas que la tension réelle, ce qui ajoutait
 * environ 4 °C. On utilise donc la calibration d'ESP-IDF, qui convertit la lecture en vraie tension (mV) grâce à
 * des mesures faites en usine sur chaque puce. Pour le potentiomètre ou la photorésistance, ce n'est pas nécessaire :
 * on compare seulement des valeurs entre elles. Ici, on veut une vraie température.
 *
 * Pour obtenir la température, on fait deux étapes :
 *   1. Retrouver la résistance de la thermistance à partir de sa part des 3.3V :
 *      Rt = 10 kΩ × tension / (3.3V - tension).
 *   2. Passer de la résistance à la température avec la formule fournie par le fabricant (celle du tutoriel Freenove) :
 *        1/T = 1/T0 + ln(Rt/R0) / B
 *      avec T0 = 25 °C (298.15 K), R0 = 10 kΩ (résistance à 25 °C) et B = 3950 (donné par le fabricant).
 *
 * À noter : chaque thermistor_init crée sa propre unité ADC. On ne peut donc pas (pour l'instant) utiliser
 * deux composants sur la même unité ADC en même temps.
 */

#include "thermistor.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "components/thermistor";

#define THERMISTOR_VCC_MV 3300      // Tension d'alimentation du diviseur (mV)
#define THERMISTOR_R0_KOHM 10.0f   // Résistance de la thermistance à 25 °C (kΩ), égale à la résistance fixe
#define THERMISTOR_T0_K 298.15f     // 25 °C en Kelvin
#define THERMISTOR_B 3950.0f        // Coefficient B de la thermistance

thermistor_t thermistor_init(gpio_num_t gpio)
{
    // On retrouve l'unité (ADC1 ou ADC2) et le canal qui correspondent au GPIO.
    adc_unit_t unit;
    adc_channel_t channel;
    adc_oneshot_io_to_channel(gpio, &unit, &channel);

    ESP_LOGI(TAG, "Configuration du GPIO %d en entrée analogique (ADC%d, canal %d)", gpio, unit + 1, channel);

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = unit,
    };
    thermistor_t thermistor = { .adc_handle = NULL, .channel = channel };
    adc_oneshot_new_unit(&unit_config, &thermistor.adc_handle);

    adc_oneshot_chan_cfg_t channel_config = {
        .bitwidth = ADC_BITWIDTH_12, // valeurs 0..4095
        .atten = ADC_ATTEN_DB_12,    // plage ~0..3.3V
    };
    adc_oneshot_config_channel(thermistor.adc_handle, channel, &channel_config);

    // Calibration (schéma « line fitting », le seul disponible sur l'ESP32)
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
        .default_vref = 1100, // utilisé seulement si la puce n'a pas de données de calibration en usine
    };
    adc_cali_create_scheme_line_fitting(&cali_config, &thermistor.cali_handle);

    return thermistor;
}

int thermistor_read_raw(thermistor_t *thermistor)
{
    if (thermistor == NULL) {
        return 0;
    }

    int raw = 0;
    adc_oneshot_read(thermistor->adc_handle, thermistor->channel, &raw);
    return raw;
}

float thermistor_read_celsius(thermistor_t *thermistor)
{
    // Vraie tension en mV, grâce à la calibration
    int mv = 0;
    adc_oneshot_get_calibrated_result(thermistor->adc_handle, thermistor->cali_handle, thermistor->channel, &mv);

    // À 3.3V, on diviserait par zéro plus bas.
    if (mv >= THERMISTOR_VCC_MV) {
        mv = THERMISTOR_VCC_MV - 1;
    }

    // Étape 1 : résistance de la thermistance, à partir de sa part des 3.3V
    float rt_kohm = THERMISTOR_R0_KOHM * mv / (THERMISTOR_VCC_MV - mv);

    // Étape 2 : température en Kelvin, avec la formule du fabricant
    float temp_k = 1.0f / (1.0f / THERMISTOR_T0_K + logf(rt_kohm / THERMISTOR_R0_KOHM) / THERMISTOR_B);

    // Conversion en Celsius
    return temp_k - 273.15f;
}
