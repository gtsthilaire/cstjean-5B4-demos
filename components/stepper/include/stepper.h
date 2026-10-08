#ifndef STEPPER_H
#define STEPPER_H

#include "driver/gpio.h"

#define STEPPER_STEPS_PER_TURN 2048 // Nombre de pas pour un tour complet (moteur 28BYJ-48 : 32 pas × réduction 64)
#define STEPPER_COILS 4             // Nombre de bobines (et de GPIO)

typedef struct {
    gpio_num_t gpios[STEPPER_COILS];
    int coil; // bobine actuellement alimentée (0..STEPPER_COILS - 1)
} stepper_t;

// Configure les 4 GPIO du driver (IN1 à IN4 du ULN2003) en sortie et retourne un stepper_t.
stepper_t stepper_init(gpio_num_t gpio_in1, gpio_num_t gpio_in2, gpio_num_t gpio_in3, gpio_num_t gpio_in4);

// Avance du nombre de pas donné : positif = un sens, négatif = l'autre sens.
// delay_ms = temps d'attente entre deux pas (plus il est court, plus le moteur tourne vite ; minimum 3 ms).
// La fonction attend que le mouvement soit terminé avant de retourner.
void stepper_move(stepper_t *stepper, int steps, int delay_ms);

#endif // STEPPER_H
