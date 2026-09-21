#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define POTENTIOMETER_MAX_RAW 4095 // Valeur maximale de l'ADC (12 bits : 2^12 - 1)

typedef struct {
    adc_oneshot_unit_handle_t adc_handle; // unité ADC (ADC1 ou ADC2) utilisée pour la lecture
    adc_channel_t channel;                // canal ADC correspondant au GPIO
} potentiometer_t;

// Configure l'ADC pour le GPIO donné (l'unité et le canal sont déduits du GPIO) et retourne un potentiometer_t.
potentiometer_t potentiometer_init(gpio_num_t gpio);

// Retourne la valeur brute lue (0..POTENTIOMETER_MAX_RAW).
int potentiometer_read_raw(potentiometer_t *potentiometer);

#endif // POTENTIOMETER_H
