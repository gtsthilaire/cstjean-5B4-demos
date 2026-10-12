#ifndef DHT11_H
#define DHT11_H

#include <stdbool.h>
#include "driver/gpio.h"

typedef struct {
    gpio_num_t gpio;
} dht11_t;

// Configure le GPIO de données du DHT11 et retourne un dht11_t.
dht11_t dht11_init(gpio_num_t gpio);

// Lit la température (°C) et l'humidité (%). Retourne false si la lecture a échoué.
// Attention : attendre au moins 1 seconde entre deux lectures (le capteur est lent).
bool dht11_read(dht11_t *dht11, int *temperature, int *humidity);

#endif // DHT11_H
