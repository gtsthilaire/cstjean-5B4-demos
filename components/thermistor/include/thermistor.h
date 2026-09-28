#ifndef THERMISTOR_H
#define THERMISTOR_H

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"

#define THERMISTOR_MAX_RAW 4095 // Valeur maximale de l'ADC (12 bits : 2^12 - 1)

typedef struct {
    adc_oneshot_unit_handle_t adc_handle; // unité ADC (ADC1 ou ADC2) utilisée pour la lecture
    adc_channel_t channel;                // canal ADC correspondant au GPIO
    adc_cali_handle_t cali_handle;        // calibration : convertit la lecture en vraie tension (mV)
} thermistor_t;

// Configure l'ADC pour le GPIO donné (l'unité et le canal sont déduits du GPIO) et retourne un thermistor_t.
thermistor_t thermistor_init(gpio_num_t gpio);

// Retourne la valeur brute lue (0..THERMISTOR_MAX_RAW).
int thermistor_read_raw(thermistor_t *thermistor);

// Retourne la température approximative en degrés Celsius.
float thermistor_read_celsius(thermistor_t *thermistor);

#endif // THERMISTOR_H
