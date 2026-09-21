#ifndef LCD1602_H
#define LCD1602_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"

#define LCD1602_COLS 16 // Nombre de colonnes visibles
#define LCD1602_ROWS 2  // Nombre de lignes

typedef struct {
    i2c_master_dev_handle_t i2c_dev; // périphérique I2C (le module PCF8574 à l'arrière de l'écran)
} lcd1602_t;

// Crée le bus I2C, ajoute le module PCF8574 à l'adresse donnée (souvent 0x27 ou 0x3F),
// initialise l'écran en mode 4 bits et retourne un lcd1602_t.
lcd1602_t lcd1602_init(gpio_num_t sda, gpio_num_t scl, uint8_t i2c_addr);

// Efface l'écran et ramène le curseur en haut à gauche.
void lcd1602_clear(lcd1602_t *lcd);

// Positionne le curseur (row: 0..1, col: 0..15). Les valeurs hors limites sont ramenées dans les limites.
void lcd1602_set_cursor(lcd1602_t *lcd, int row, int col);

// Écrit une chaîne à partir de la position du curseur.
void lcd1602_print(lcd1602_t *lcd, const char *str);

// Décale tout l'affichage d'une colonne vers la gauche (le contenu de la mémoire de l'écran ne change pas).
void lcd1602_scroll_left(lcd1602_t *lcd);

#endif // LCD1602_H
