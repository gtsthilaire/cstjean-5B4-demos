/**
 * Un module WS2812 (aussi appelé NeoPixel) contient plusieurs LED RGB adressables individuellement,
 * branchées en chaîne sur un seul fil de données. Chaque LED contient une petite puce qui garde sa couleur,
 * puis transmet le reste des données à la LED suivante.
 *
 * Les WS2812 ont besoin d'impulsions très précises (de l'ordre de 0.4 µs) pour distinguer les 0 et les 1.
 * On utilise donc le périphérique RMT de l'ESP32, qui génère ces signaux automatiquement.
 *
 * Les WS2812 attendent les couleurs dans l'ordre vert, rouge, bleu (G-R-B) et non R-G-B. Le composant led_strip
 * s'occupe de réordonner les octets (format GRB par défaut), c'est pourquoi on peut garder une API en r, g, b.
 *
 * Chaque LED consomme jusqu'à ~60 mA en blanc à pleine intensité, soit ~480 mA pour 8 LED : c'est éblouissant et
 * ça demande beaucoup au port USB. On applique donc une luminosité globale (brightness) à toutes les couleurs,
 * comme la fonction setBrightness du tutoriel Freenove.
 *
 * On utilise le composant led_strip d'Espressif, déclaré dans idf_component.yml :
 * https://components.espressif.com/components/espressif/led_strip
 * https://espressif.github.io/idf-extra-components/latest/led_strip/index.html
 */

#include "led_pixel.h"
#include "esp_log.h"

static const char *TAG = "components/led_pixel";

#define LED_PIXEL_RMT_RES_HZ (10 * 1000 * 1000) // Résolution du RMT : 10 MHz, soit une précision de 0.1 µs

// Applique la luminosité globale à une composante de couleur (0..255)
static uint8_t apply_brightness(const led_pixel_t *led_pixel, uint8_t value)
{
    return (uint8_t)((uint32_t)value * led_pixel->brightness / 255);
}

led_pixel_t led_pixel_init(gpio_num_t gpio, size_t count)
{
    ESP_LOGI(TAG, "Configuration du module WS2812 sur le GPIO %d (%d LED)", gpio, (int)count);

    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio,
        .max_leds = count,
        .led_model = LED_MODEL_WS2812,
    };

    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_PIXEL_RMT_RES_HZ,
    };

    led_pixel_t led_pixel = { .strip = NULL, .count = count, .brightness = 255 };
    esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_pixel.strip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Impossible de configurer le module WS2812 : %s", esp_err_to_name(err));
        led_pixel.strip = NULL;
        return led_pixel;
    }

    led_strip_clear(led_pixel.strip);

    return led_pixel;
}

void led_pixel_set_brightness(led_pixel_t *led_pixel, uint8_t brightness)
{
    if (led_pixel == NULL) {
        return;
    }

    led_pixel->brightness = brightness;
}

void led_pixel_set(led_pixel_t *led_pixel, size_t index, uint8_t r, uint8_t g, uint8_t b)
{
    if (led_pixel == NULL || led_pixel->strip == NULL || index >= led_pixel->count) {
        return;
    }

    led_strip_set_pixel(led_pixel->strip, index,
                        apply_brightness(led_pixel, r),
                        apply_brightness(led_pixel, g),
                        apply_brightness(led_pixel, b));
}

// Le HSV décrit une couleur par sa teinte (hue) plutôt que par ses composantes rouge, verte et bleue.
// La teinte est un angle sur la roue des couleurs : 0 = rouge, 60 = jaune, 120 = vert, 180 = cyan, 240 = bleu,
// 300 = magenta, puis on revient au rouge à 360. C'est pratique pour faire un arc-en-ciel : il suffit de faire
// varier un seul nombre. La saturation est au maximum (couleur pure) et la valeur (intensité) = luminosité globale.
void led_pixel_set_hsv(led_pixel_t *led_pixel, size_t index, uint16_t hue)
{
    if (led_pixel == NULL || led_pixel->strip == NULL || index >= led_pixel->count) {
        return;
    }

    led_strip_set_pixel_hsv(led_pixel->strip, index, hue % 360, 255, led_pixel->brightness);
}

void led_pixel_show(led_pixel_t *led_pixel)
{
    if (led_pixel == NULL || led_pixel->strip == NULL) {
        return;
    }

    led_strip_refresh(led_pixel->strip);
}

void led_pixel_clear(led_pixel_t *led_pixel)
{
    if (led_pixel == NULL || led_pixel->strip == NULL) {
        return;
    }

    led_strip_clear(led_pixel->strip);
}
