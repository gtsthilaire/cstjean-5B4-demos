#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "driver/rmt_rx.h"

#define IR_REMOTE_MAX_SYMBOLS 64 // Une trame NEC complète contient 34 impulsions, on garde de la marge

typedef struct {
    rmt_channel_handle_t channel;                     // canal RMT qui mesure les impulsions reçues
    rmt_symbol_word_t symbols[IR_REMOTE_MAX_SYMBOLS]; // durées des impulsions de la dernière trame
} ir_remote_t;

// Configure le récepteur infrarouge sur le GPIO donné et retourne un ir_remote_t.
ir_remote_t ir_remote_init(gpio_num_t gpio);

// Attend qu'une touche de la télécommande soit pressée, puis place son code dans *code (ex : 0xFF6897 pour « 0 »).
// Retourne false si la trame reçue n'est pas valide (parasite, touche maintenue enfoncée, etc.).
bool ir_remote_read(ir_remote_t *ir_remote, uint32_t *code);

#endif // IR_REMOTE_H
