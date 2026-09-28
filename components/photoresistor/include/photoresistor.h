#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define PHOTORESISTOR_MAX_RAW 4095 // Valeur maximale de l'ADC (12 bits : 2^12 - 1)

typedef struct {
    adc_oneshot_unit_handle_t adc_handle; // unité ADC (ADC1 ou ADC2) utilisée pour la lecture
    adc_channel_t channel;                // canal ADC correspondant au GPIO
} photoresistor_t;

// Configure l'ADC pour le GPIO donné (l'unité et le canal sont déduits du GPIO) et retourne un photoresistor_t.
photoresistor_t photoresistor_init(gpio_num_t gpio);

// Retourne la valeur brute lue (0..PHOTORESISTOR_MAX_RAW). Plus il y a de lumière, plus la valeur est BASSE.
int photoresistor_read_raw(photoresistor_t *photoresistor);

// Retourne le niveau de lumière en pourcentage (0 = noir, 100 = très éclairé).
int photoresistor_read_percent(photoresistor_t *photoresistor);

#endif // PHOTORESISTOR_H
