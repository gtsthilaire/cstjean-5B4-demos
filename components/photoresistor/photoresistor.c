/**
 * Démonstration de l'utilisation d'une photorésistance.
 * Une photorésistance est un composant dont la résistance varie en fonction de la lumière.
 *
 * ** Voir le component potentiometer pour une explication plus détaillée sur le ADC/DAC. **
 *
 * Tutoriel original : Chapitre 12 - Photoresistor (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de gestion de LED en fonction de la luminosité ambiante.
 */

#include "photoresistor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "photoresistor.c";

static adc_channel_t s_photoresistor_channel = -1;

static adc_oneshot_unit_handle_t s_adc_handle;

static void photoresistor_init(adc_channel_t channel)
{
    s_photoresistor_channel = channel;

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&unit_config, &s_adc_handle);

    adc_oneshot_chan_cfg_t ch_config = {
        .bitwidth = ADC_BITWIDTH_12,     // valeurs 0..4095
        .atten = ADC_ATTEN_DB_11,        // plage ~0..3.3V
    };
    adc_oneshot_config_channel(s_adc_handle, s_photoresistor_channel, &ch_config);
}

static void photoresistor_task(void *arg)
{
    while (1) {
        int adc_raw = 0;
        adc_oneshot_read(s_adc_handle, s_photoresistor_channel, &adc_raw);

        double voltage = (adc_raw / 4095.0) * 3.3;  // approximation simple

        ESP_LOGI(TAG, "ADC(GPIO34): raw=%4d   V≈ %.2f V", adc_raw, voltage);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_photoresistor_task(adc_channel_t channel)
{
    photoresistor_init(channel);

    xTaskCreate(photoresistor_task, "photoresistor_task", 2048, NULL, 5, NULL);
}