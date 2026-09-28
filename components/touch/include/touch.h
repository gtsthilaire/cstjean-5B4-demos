#ifndef TOUCH_H
#define TOUCH_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/touch_sens.h"

typedef struct {
    touch_sensor_handle_t sensor;   // contrôleur tactile (un seul pour tout l'ESP32)
    touch_channel_handle_t channel; // canal tactile correspondant au GPIO
    uint32_t touch_threshold;       // seuil bas : sous cette valeur, le capteur devient touché
    uint32_t release_threshold;     // seuil haut : au-dessus de cette valeur, le capteur redevient relâché
    bool touched;                   // dernier état retourné par touch_is_touched
} touch_t;

// Configure le capteur tactile pour le GPIO donné (le canal est déduit du GPIO), mesure la valeur au repos
// pour calculer les seuils, puis démarre les mesures en continu. Ne pas toucher le capteur pendant l'init.
touch_t touch_init(gpio_num_t gpio);

// Retourne la valeur mesurée (lissée). Elle diminue quand on touche le capteur.
uint32_t touch_read(touch_t *touch);

// Retourne true si le capteur est touché, false sinon (avec deux seuils pour éviter le clignotement, voir touch.c).
bool touch_is_touched(touch_t *touch);

#endif // TOUCH_H
