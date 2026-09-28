/**
 * Afficheur 7 segments contrôlé avec un 74HC595.
 * Voir components/led_bar_74hc595 pour le fonctionnement du 74HC595.
 *
 * Un afficheur 7 segments contient 8 LED en forme de barres (7 segments + le point). En allumant les bons
 * segments, on forme un chiffre. Le 74HC595 a justement 8 sorties : une par segment.
 *
 *        a
 *      -----
 *   f |     | b
 *     |  g  |
 *      -----
 *   e |     | c
 *     |     |
 *      -----   ● point
 *        d
 *
 * Chaque bit de l'octet envoyé correspond à un segment : bit 0 = a, bit 1 = b, ..., bit 6 = g, bit 7 = point.
 * Ex : pour afficher 1, on allume b et c → 00000110 = 0x06.
 *
 * L'afficheur du kit est à anode commune : un segment s'allume quand sa sortie est à LOW (même principe que la
 * LED RGB du chapitre 5). On écrit donc la table avec 1 = segment allumé (plus lisible), et on inverse tous
 * les bits (~) juste avant de l'envoyer.
 */

#include "seven_segment.h"
#include "esp_log.h"

static const char *TAG = "components/seven_segment";

// Segments à allumer pour chaque chiffre (1 = allumé). Bits : . g f e d c b a
static const uint8_t DIGITS[] = {
    0x3F, // 0 : 00111111 → a b c d e f
    0x06, // 1 : 00000110 → b c
    0x5B, // 2 : 01011011 → a b d e g
    0x4F, // 3 : 01001111 → a b c d g
    0x66, // 4 : 01100110 → b c f g
    0x6D, // 5 : 01101101 → a c d f g
    0x7D, // 6 : 01111101 → a c d e f g
    0x07, // 7 : 00000111 → a b c
    0x7F, // 8 : 01111111 → tous sauf le point
    0x6F, // 9 : 01101111 → a b c d f g
    0x77, // A : 01110111 → a b c e f g
    0x7C, // b : 01111100 → c d e f g
    0x39, // C : 00111001 → a d e f
    0x5E, // d : 01011110 → b c d e g
    0x79, // E : 01111001 → a d e f g
    0x71, // F : 01110001 → a e f g
};
#define DIGITS_COUNT (sizeof(DIGITS) / sizeof(DIGITS[0]))

// Envoie un octet au 74HC595, bit par bit, en commençant par le bit de poids faible (bit 0), comme dans le tutoriel.
// L'ordre dépend du câblage : le premier bit envoyé finit sur la dernière sortie du 74HC595.
static void shift_out(seven_segment_t *seven_segment, uint8_t value)
{
    for (int i = 0; i < 8; i++) {
        gpio_set_level(seven_segment->gpio_data, (value >> i) & 0x01); // On place le bit i sur Data (le & 0x01 ne garde que ce bit)
        gpio_set_level(seven_segment->gpio_clock, 1);                  // Front montant : le registre lit le bit
        gpio_set_level(seven_segment->gpio_clock, 0);                  // On prépare la prochaine impulsion
    }
}

// Envoie les segments à allumer (1 = allumé) au 74HC595.
static void show_segments(seven_segment_t *seven_segment, uint8_t segments)
{
    gpio_set_level(seven_segment->gpio_latch, 0); // On désactive le latch pour commencer à envoyer les données
    shift_out(seven_segment, ~segments);          // Inversion : anode commune, un segment s'allume à LOW
    gpio_set_level(seven_segment->gpio_latch, 1); // On active le latch : tous les segments changent en même temps
}

seven_segment_t seven_segment_init(gpio_num_t gpio_data, gpio_num_t gpio_clock, gpio_num_t gpio_latch)
{
    ESP_LOGI(TAG, "Configuration des GPIO %d (data), %d (clock) et %d (latch) en sortie", gpio_data, gpio_clock, gpio_latch);

    // On configure les 3 broches d'un coup avec un masque de bits
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << gpio_data) | (1ULL << gpio_clock) | (1ULL << gpio_latch),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_config);

    seven_segment_t seven_segment = {
        .gpio_data = gpio_data,
        .gpio_clock = gpio_clock,
        .gpio_latch = gpio_latch,
    };

    gpio_set_level(gpio_latch, 0);
    gpio_set_level(gpio_clock, 0);
    gpio_set_level(gpio_data, 0);

    show_segments(&seven_segment, 0x00); // On commence avec l'afficheur éteint

    return seven_segment;
}

void seven_segment_show_digit(seven_segment_t *seven_segment, int digit)
{
    if (seven_segment == NULL) {
        return;
    }

    if (digit < 0 || digit >= (int)DIGITS_COUNT) {
        show_segments(seven_segment, 0x00); // Hors plage : on éteint
        return;
    }

    show_segments(seven_segment, DIGITS[digit]);
}
