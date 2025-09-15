/**
 * Démonstration de l'utilisation d'un module WS2812 (NeoPixel).
 * Un WS2812 est plusieurs LEDs RGB adressablent individuellement.
 * Dans l'exemple, les pixels LED s'allument dans différentes couleurs.
 * 
 * Voir main/idf_component.yml pour les dépendances.
 * https://components.espressif.com/components/espressif/led_strip
 * https://espressif.github.io/idf-extra-components/latest/led_strip/index.html
 * 
 * Tutoriel original : Chapitre 6 - LED pixel (Freenove)
 */

#include "led_pixel.h"
#include "led_strip.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "led_pixel.c";

static led_strip_handle_t s_led_strip;

#define LED_STRIP_LED_COUNT 8           // Nombre de LEDs dans le module WS2812
#define LED_STRIP_RMT_RES_HZ 10000000   // Résolution RMT (10MHz pour WS2812)

static void led_pixel_init(gpio_num_t gpio)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = gpio,
        .max_leds = LED_STRIP_LED_COUNT,
        .led_model = LED_MODEL_WS2812,
    };

    // Les LEDs WS2812 ont besoin d’impulsions précises pour comprendre les 0 et 1.
    // Le périphérique RMT génère ces signaux automatiquement.
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = LED_STRIP_RMT_RES_HZ,
    };
    led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
}

static void led_pixel_task(void *arg)
{
    bool led_on_off = false;

    int colors[5][3] = {
        {255, 0, 0},    // Rouge
        {0, 255, 0},    // Vert
        {0, 0, 255},    // Bleu
        {255, 255, 0},  // Jaune
        {0, 255, 255}   // Cyan
    };

    while (1) {
        if (led_on_off) {
            ESP_LOGD(TAG, "Led ON");

            // Allumer les LEDs une par une dans différentes couleurs
            for (int j = 0; j < 5; j++) {
                for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
                    led_strip_set_pixel(s_led_strip, i, colors[j][0], colors[j][1], colors[j][2]);
                    led_strip_refresh(s_led_strip);
                    vTaskDelay(pdMS_TO_TICKS(500));
                }               
            }
        } else {
            ESP_LOGD(TAG, "Led OFF");
            led_strip_clear(s_led_strip);
        }

        led_on_off = !led_on_off;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_led_pixel_task(gpio_num_t gpio)
{
    led_pixel_init(gpio);
    
    xTaskCreate(led_pixel_task, "led_pixel_task", 2048, NULL, 5, NULL);
}