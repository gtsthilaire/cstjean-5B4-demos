/**
 * Démonstration de l'utilisation d'un potentiomètre.
 * Le potentiomètre permet de varier une tension analogique en fonction de sa position.
 *
 * Le ADC (Analog to Digital Converter) est utilisé pour lire la valeur du potentiomètre. 
 * Il convertit une tension analogique en une valeur numérique (ici sur 12 bits, soit de 0 à 4095).
 * La valeur lue est ensuite convertie en une tension approximative. Cette tension peut être utilisée
 * pour contrôler d'autres composants, comme un DAC (Digital to Analog Converter) ou un signal PWM.
 *
 * Sur ESP32, les broches ADC1 disponibles sont :
 * GPIO36 (ADC1_CH0), GPIO39 (ADC1_CH3), GPIO32 (ADC1_CH4), GPIO33 (ADC1_CH5), GPIO34 (ADC1_CH6), GPIO35 (ADC1_CH7).
 * 
 * Les broches ADC2 sont partagées avec le WiFi, donc il est préférable d'utiliser ADC1.
 * 
 * Les broches DAC disponibles sont : 
 * GPIO25 (DAC1) et GPIO26 (DAC2).
 * 
 * Tutoriel original : Chapitre 9 - AD/DA Converter (Freenove)
 */

#include "potentiometer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "potentiometer.c";

static adc_channel_t s_potentiometer_channel = -1;

static adc_oneshot_unit_handle_t s_adc_handle;

static void potentiometer_init(adc_channel_t channel)
{
    s_potentiometer_channel = channel;

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&unit_config, &s_adc_handle);

    adc_oneshot_chan_cfg_t ch_config = {
        .bitwidth = ADC_BITWIDTH_12,     // valeurs 0..4095
        .atten = ADC_ATTEN_DB_11,        // plage ~0..3.3V
    };
    adc_oneshot_config_channel(s_adc_handle, s_potentiometer_channel, &ch_config);
}

static void potentiometer_task(void *arg)
{
    while (1) {
        int adc_raw = 0;
        adc_oneshot_read(s_adc_handle, s_potentiometer_channel, &adc_raw);

        double voltage = (adc_raw / 4095.0) * 3.3;  // approximation simple

        ESP_LOGD(TAG, "ADC(GPIO34): raw=%4d   V≈ %.2f V", adc_raw, voltage);

        // Utiliser DAC (GPIO25 ou GPIO26) ou PWM pour générer une tension de sortie
        // en fonction de la valeur lue sur le potentiomètre

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_potentiometer_task(adc_channel_t channel)
{
    potentiometer_init(channel);
    
    xTaskCreate(potentiometer_task, "potentiometer_task", 2048, NULL, 5, NULL);
}