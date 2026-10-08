#ifndef RELAY_H
#define RELAY_H

#include <stdbool.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio;
} relay_t;

// Configure le GPIO en sortie et retourne un relay_t. Le relais démarre ouvert (circuit coupé).
relay_t relay_init(gpio_num_t gpio);

// Ferme (true) ou ouvre (false) le relais : true = le courant passe dans le circuit commandé.
void relay_set(relay_t *relay, bool on);

#endif // RELAY_H
