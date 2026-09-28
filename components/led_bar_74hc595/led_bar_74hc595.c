/**
 * Barre de LED contrôlée avec un 74HC595.
 *
 * Un 74HC595 permet de contrôler 8 sorties numériques en utilisant seulement 3 broches GPIO.
 * À comparer avec la démo 03, où chaque LED de la barre utilise son propre GPIO (10 GPIO pour 10 LED).
 * Comme le 74HC595 n'a que 8 sorties, seules 8 des 10 LED de la barre sont utilisées.
 *
 * Le 74HC595 est un registre à décalage (shift register). Il permet de convertir des données série en données
 * parallèles. Il fonctionne comme une file de bits : on lui envoie un bit à la fois, il décale les précédents
 * et au bout de 8 bits, on a une série de bits stockés à l'intérieur, un par sortie.
 *
 * Il fonctionne avec 3 GPIO :
 * - Données (Data) : les bits sont envoyés un par un sur cette broche. Ils sont stockés dans le registre.
 * - Horloge (Clock) : chaque impulsion fait entrer le bit présent sur Data dans le registre.
 * - Verrou (Latch) : il copie le contenu du registre sur les sorties (les LED changent à ce moment-là).
 *
 * Le verrou évite de voir les LED « clignoter » pendant l'envoi : les 8 LED changent toutes en même temps,
 * une fois que les 8 bits sont arrivés.
 */

#include "led_bar_74hc595.h"
#include "esp_log.h"

static const char *TAG = "components/led_bar_74hc595";

// Envoie un octet au 74HC595, bit par bit, en commençant par le bit de poids faible (bit 0), comme dans le tutoriel.
// L'ordre dépend du câblage : le premier bit envoyé finit sur la dernière sortie du 74HC595.
static void shift_out(led_bar_74hc595_t *led_bar_74hc595, uint8_t value)
{
    for (int i = 0; i < 8; i++) {
        gpio_set_level(led_bar_74hc595->gpio_data, (value >> i) & 0x01); // On place le bit i sur Data (le & 0x01 ne garde que ce bit)
        gpio_set_level(led_bar_74hc595->gpio_clock, 1);                  // Front montant : le registre lit le bit
        gpio_set_level(led_bar_74hc595->gpio_clock, 0);                  // On prépare la prochaine impulsion
    }
}

led_bar_74hc595_t led_bar_74hc595_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch)
{
    ESP_LOGI(TAG, "Configuration des GPIO %d (data), %d (clock) et %d (latch) en sortie", gpio_data, gpio_clock, gpio_latch);

    // On configure les 3 broches d'un coup avec un masque de bits
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << gpio_data) | (1ULL << gpio_clock) | (1ULL << gpio_latch),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_config);

    led_bar_74hc595_t led_bar_74hc595 = {
        .gpio_data = gpio_data,
        .gpio_clock = gpio_clock,
        .gpio_latch = gpio_latch,
    };

    gpio_set_level(gpio_latch, 0);
    gpio_set_level(gpio_clock, 0);
    gpio_set_level(gpio_data, 0);

    led_bar_74hc595_show(&led_bar_74hc595, 0x00); // On commence avec toutes les LED éteintes

    return led_bar_74hc595;
}

void led_bar_74hc595_show(led_bar_74hc595_t *led_bar_74hc595, uint8_t leds)
{
    if (led_bar_74hc595 == NULL) {
        return;
    }

    gpio_set_level(led_bar_74hc595->gpio_latch, 0); // On désactive le latch pour commencer à envoyer les données
    shift_out(led_bar_74hc595, leds);
    gpio_set_level(led_bar_74hc595->gpio_latch, 1); // On active le latch : les LED changent toutes en même temps
}
