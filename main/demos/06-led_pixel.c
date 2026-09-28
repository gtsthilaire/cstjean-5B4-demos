#include "led_pixel.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/06-led_pixel";

#define LED_PIXEL_GPIO GPIO_NUM_2
#define LED_PIXEL_COUNT 8         // Nombre de LED dans le module du kit
#define LED_PIXEL_BRIGHTNESS 50   // Luminosité globale (0..255) : 255 est éblouissant et consomme beaucoup

#define RAINBOW_STEPS 250         // Nombre de pas de l'arc-en-ciel (250 x 20 ms = 5 s)
#define RAINBOW_HUE_STEP 5        // Décalage de la teinte à chaque pas (en degrés)

typedef struct {
    const char *name;
    uint8_t r, g, b;
} color_t;

static const color_t COLORS[] = {
    { "ROUGE", 255,   0,   0 },
    { "VERT",    0, 255,   0 },
    { "BLEU",    0,   0, 255 },
    { "JAUNE", 255, 255,   0 },
    { "CYAN",    0, 255, 255 },
};

// Arc-en-ciel qui tourne (projet 6.2 du tutoriel) : les LED sont réparties sur la roue des couleurs
// (chacune décalée de 360 / count degrés), puis on fait tourner la roue en augmentant la teinte de départ.
static void rainbow(led_pixel_t *led_pixel)
{
    uint16_t offset = 0;

    for (int step = 0; step < RAINBOW_STEPS; step++) {
        for (size_t i = 0; i < led_pixel->count; i++) {
            led_pixel_set_hsv(led_pixel, i, (offset + i * 360 / led_pixel->count) % 360);
        }
        led_pixel_show(led_pixel);

        offset = (offset + RAINBOW_HUE_STEP) % 360;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void led_pixel_task(void *arg)
{
    led_pixel_t *led_pixel = (led_pixel_t *)arg;

    while (1) {
        // Allumer les LED une par une, dans chaque couleur
        for (size_t c = 0; c < sizeof(COLORS) / sizeof(COLORS[0]); c++) {
            ESP_LOGI(TAG, "%s", COLORS[c].name);
            for (size_t i = 0; i < led_pixel->count; i++) {
                led_pixel_set(led_pixel, i, COLORS[c].r, COLORS[c].g, COLORS[c].b);
                led_pixel_show(led_pixel);
                vTaskDelay(pdMS_TO_TICKS(100));
            }
        }

        ESP_LOGI(TAG, "ARC-EN-CIEL");
        rainbow(led_pixel);

        ESP_LOGI(TAG, "LED OFF");
        led_pixel_clear(led_pixel);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void start_demo_06_led_pixel(void)
{
    static led_pixel_t led_pixel;
    led_pixel = led_pixel_init(LED_PIXEL_GPIO, LED_PIXEL_COUNT);
    led_pixel_set_brightness(&led_pixel, LED_PIXEL_BRIGHTNESS);

    xTaskCreate(led_pixel_task, "06-led_pixel_task", 2048, &led_pixel, 1, NULL);
}
