#ifndef KEYPAD_H
#define KEYPAD_H

#include "driver/gpio.h"

#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

typedef struct {
    gpio_num_t gpio_rows[KEYPAD_ROWS];
    gpio_num_t gpio_cols[KEYPAD_COLS];
    char last_key; // touche enfoncée lors du dernier balayage (0 = aucune)
} keypad_t;

// Configure les rangées en sortie et les colonnes en entrée (avec pull-up), puis retourne un keypad_t.
keypad_t keypad_init(const gpio_num_t gpio_rows[KEYPAD_ROWS], const gpio_num_t gpio_cols[KEYPAD_COLS]);

// Retourne la touche qui vient d'être enfoncée ('0'..'9', 'A'..'D', '*', '#'), ou 0 s'il n'y en a pas.
// Une touche maintenue enfoncée n'est retournée qu'une seule fois. À appeler régulièrement (ex : toutes les 20 ms).
char keypad_get_key(keypad_t *keypad);

#endif // KEYPAD_H
