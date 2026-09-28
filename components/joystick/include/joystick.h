#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "push_button.h"

#define JOYSTICK_MAX_RAW 4095 // Valeur maximale de l'ADC (12 bits : 2^12 - 1)

typedef struct {
    adc_oneshot_unit_handle_t adc_handle; // unité ADC partagée par les axes X et Y
    adc_channel_t channel_x;              // canal ADC de l'axe X
    adc_channel_t channel_y;              // canal ADC de l'axe Y
    push_button_t button;                 // bouton Z (on appuie sur le joystick)
    int center_x;                         // valeur brute de X au repos (mesurée au démarrage)
    int center_y;                         // valeur brute de Y au repos (mesurée au démarrage)
} joystick_t;

// Configure les axes X et Y (entrées analogiques, sur la même unité ADC) et le bouton Z, mesure la position au repos
// (le centre), puis retourne un joystick_t. Ne pas toucher le joystick pendant l'init.
joystick_t joystick_init(gpio_num_t gpio_x, gpio_num_t gpio_y, gpio_num_t gpio_z);

// Lit la position brute des axes X et Y (0..JOYSTICK_MAX_RAW, environ la moitié au repos).
void joystick_read(joystick_t *joystick, int *x, int *y);

// Lit la position des axes X et Y en pourcentage : 0 = centre, -100 / +100 = au bout (avec une petite zone morte).
// Droite = X positif, haut = Y positif.
void joystick_read_percent(joystick_t *joystick, int *x, int *y);

// Retourne true si le bouton Z est pressé, false sinon (avec debounce logiciel).
bool joystick_is_pressed(joystick_t *joystick);

#endif // JOYSTICK_H
