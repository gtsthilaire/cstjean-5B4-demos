/**
 * Un clavier matriciel 4x4 a 16 touches, mais seulement 8 fils : 4 rangées et 4 colonnes.
 * Chaque touche relie une rangée à une colonne quand on l'enfonce.
 *
 * Pour savoir quelle touche est enfoncée, on « balaie » le clavier :
 *   - toutes les rangées sont à l'état haut, sauf une qu'on met à l'état bas ;
 *   - on lit les colonnes : une colonne à l'état bas = la touche à la croisée de cette rangée et de cette colonne ;
 *   - on passe à la rangée suivante.
 *
 * C'est la même idée que la matrice de LED (voir components/led_matrix), mais en lecture plutôt qu'en écriture.
 *
 * Les colonnes utilisent la résistance de pull-up interne de l'ESP32 : elles restent à l'état haut tant
 * qu'aucune touche ne les relie à une rangée à l'état bas.
 */

#include "keypad.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

static const char *TAG = "components/keypad";

// Symbole de chaque touche, selon sa rangée et sa colonne
static const char KEYS[KEYPAD_ROWS][KEYPAD_COLS] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' },
};

keypad_t keypad_init(const gpio_num_t gpio_rows[KEYPAD_ROWS], const gpio_num_t gpio_cols[KEYPAD_COLS])
{
    ESP_LOGI(TAG, "Configuration des rangées en sortie et des colonnes en entrée");

    keypad_t keypad = { .last_key = 0 };

    for (int i = 0; i < KEYPAD_ROWS; i++) {
        keypad.gpio_rows[i] = gpio_rows[i];
        gpio_reset_pin(gpio_rows[i]);
        gpio_set_direction(gpio_rows[i], GPIO_MODE_OUTPUT);
        gpio_set_level(gpio_rows[i], 1); // rangée inactive
    }

    for (int i = 0; i < KEYPAD_COLS; i++) {
        keypad.gpio_cols[i] = gpio_cols[i];
        gpio_reset_pin(gpio_cols[i]);
        gpio_set_direction(gpio_cols[i], GPIO_MODE_INPUT);
        gpio_set_pull_mode(gpio_cols[i], GPIO_PULLUP_ONLY);
    }

    return keypad;
}

// Balaie tout le clavier et retourne la touche enfoncée (0 si aucune).
static char scan(keypad_t *keypad)
{
    char key = 0;

    for (int row = 0; row < KEYPAD_ROWS; row++) {
        gpio_set_level(keypad->gpio_rows[row], 0); // on active cette rangée
        esp_rom_delay_us(5);                        // petit délai pour que le signal se stabilise

        for (int col = 0; col < KEYPAD_COLS; col++) {
            if (gpio_get_level(keypad->gpio_cols[col]) == 0) {
                key = KEYS[row][col];
            }
        }

        gpio_set_level(keypad->gpio_rows[row], 1); // on la désactive avant de passer à la suivante
    }

    return key;
}

char keypad_get_key(keypad_t *keypad)
{
    if (keypad == NULL) {
        return 0;
    }

    char key = scan(keypad);

    // On ne retourne la touche qu'au moment où elle est enfoncée (pas tant qu'elle reste enfoncée)
    char new_key = (key != keypad->last_key) ? key : 0;
    keypad->last_key = key;

    return new_key;
}
