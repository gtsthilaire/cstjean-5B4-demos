#include "led_rgb.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/05-led_rgb";

#define LED_RGB_GPIO_R GPIO_NUM_15
#define LED_RGB_GPIO_G GPIO_NUM_2
#define LED_RGB_GPIO_B GPIO_NUM_4

#define LED_RGB_CHANNEL_R LEDC_CHANNEL_0
#define LED_RGB_CHANNEL_G LEDC_CHANNEL_1
#define LED_RGB_CHANNEL_B LEDC_CHANNEL_2

typedef struct {
    const char *name;
    uint8_t r, g, b;
} color_t;

// Couleurs primaires, puis mélanges de deux couleurs, puis des trois
static const color_t COLORS[] = {
    { "ROUGE",   255,   0,   0 },
    { "VERT",      0, 255,   0 },
    { "BLEU",      0,   0, 255 },
    { "JAUNE",   255, 255,   0 }, // rouge + vert
    { "CYAN",      0, 255, 255 }, // vert + bleu
    { "MAGENTA", 255,   0, 255 }, // rouge + bleu
    { "BLANC",   255, 255, 255 }, // les trois
};

// Fait le tour de la roue des couleurs : on passe graduellement du rouge au vert, du vert au bleu,
// puis du bleu au rouge. À chaque étape, une couleur diminue pendant que la suivante augmente.
static void color_wheel(led_rgb_t *led_rgb)
{
    for (int i = 0; i <= 255; i++) {
        led_rgb_set(led_rgb, 255 - i, i, 0); // rouge -> vert
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (int i = 0; i <= 255; i++) {
        led_rgb_set(led_rgb, 0, 255 - i, i); // vert -> bleu
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    for (int i = 0; i <= 255; i++) {
        led_rgb_set(led_rgb, i, 0, 255 - i); // bleu -> rouge
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void led_rgb_task(void *arg)
{
    led_rgb_t *led_rgb = (led_rgb_t *)arg;

    while (1) {
        for (size_t i = 0; i < sizeof(COLORS) / sizeof(COLORS[0]); i++) {
            ESP_LOGI(TAG, "%s", COLORS[i].name);
            led_rgb_set(led_rgb, COLORS[i].r, COLORS[i].g, COLORS[i].b);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        ESP_LOGI(TAG, "ROUE DES COULEURS");
        color_wheel(led_rgb);
    }
}

void start_demo_05_led_rgb(void)
{
    static led_rgb_t led_rgb;
    led_rgb = led_rgb_init(LED_RGB_GPIO_R, LED_RGB_CHANNEL_R,
                           LED_RGB_GPIO_G, LED_RGB_CHANNEL_G,
                           LED_RGB_GPIO_B, LED_RGB_CHANNEL_B);

    xTaskCreate(led_rgb_task, "05-led_rgb_task", 2048, &led_rgb, 1, NULL);
}
