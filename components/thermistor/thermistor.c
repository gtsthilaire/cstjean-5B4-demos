/**
 * Démonstration de l'utilisation d'une thermistance.
 * Une thermistance est un composant dont la résistance varie en fonction de la température.
 *
 * ** Voir le component potentiometer pour une explication plus détaillée sur le ADC/DAC. **
 *
 * Tutoriel original : Chapitre 13 - Thermistor (Freenove)
 */

#include <math.h> 
#include "thermistor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "thermistor.c";

static adc_channel_t s_thermistor_channel = -1;

static adc_oneshot_unit_handle_t s_adc_handle;

static void thermistor_init(adc_channel_t channel)
{
    s_thermistor_channel = channel;

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&unit_config, &s_adc_handle);

    adc_oneshot_chan_cfg_t ch_config = {
        .bitwidth = ADC_BITWIDTH_12,     // valeurs 0..4095
        .atten = ADC_ATTEN_DB_11,        // plage ~0..3.3V
    };
    adc_oneshot_config_channel(s_adc_handle, s_thermistor_channel, &ch_config);
}

static void thermistor_task(void *arg)
{
    while (1) {
        int adc_raw = 0;
        adc_oneshot_read(s_adc_handle, s_thermistor_channel, &adc_raw);

        double voltage = (adc_raw / 4095.0) * 3.3;  // approximation simple

        // On fait confiance à Freenove pour la formule de conversion ... !
        double Rt = (10 * voltage) / (3.3 - voltage);  // résistance de la thermistance (R = U/I)
        double tempK = 1 / (1 / (273.15 + 25) + log(Rt / 10) / 3950.0); // température en Kelvin
        double tempC = tempK - 273.15;  // conversion en Celsius

        ESP_LOGI(TAG, "ADC(GPIO34): raw=%4d   V≈ %.2f V   Temp≈ %.2f °C", adc_raw, voltage, tempC);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_thermistor_task(adc_channel_t channel)
{
    thermistor_init(channel);

    xTaskCreate(thermistor_task, "thermistor_task", 2048, NULL, 5, NULL);
}