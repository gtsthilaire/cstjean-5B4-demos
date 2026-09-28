#include "led_matrix.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/16-led_matrix";

#define LED_MATRIX_GPIO_DATA GPIO_NUM_15
#define LED_MATRIX_GPIO_CLOCK GPIO_NUM_4
#define LED_MATRIX_GPIO_LATCH GPIO_NUM_2

#define PATTERN_DELAY_MS 3000 // Durée d'affichage de chaque motif

// Mode test : mettre à 1 pour allumer toutes les LED et vérifier les branchements.
// Une rangée ou une colonne qui reste éteinte indique un fil mal branché.
#define TEST_MODE 0

static const uint8_t ALL_ON[LED_MATRIX_SIZE] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

// Première façon d'écrire un motif : avec des octets.
// Chaque octet correspond à une colonne (de 0 à 7) et chaque bit à une rangée.
//
// Visage souriant
//
//     7 6 5 4 3 2 1 0   ← colonnes
// 7   · · · · · · · ·
// 6   · · ● ● ● ● · ·
// 5   · ● · · · · ● ·
// 4   ● · ● · · ● · ●
// 3   ● · · · · · · ●
// 2   ● · · ● ● · · ●
// 1   · ● · · · · ● ·
// 0   · · ● ● ● ● · ·
// ↑ rangées
static const uint8_t SMILING_FACE[LED_MATRIX_SIZE] = {
    0x1C,   // 00011100 = · · · ● ● ● · ·
    0x22,   // 00100010 = · · ● · · · ● ·
    0x51,   // 01010001 = · ● · ● · · · ●
    0x45,   // 01000101 = · ● · · · ● · ●
    0x45,   // 01000101 = · ● · · · ● · ●
    0x51,   // 01010001 = · ● · ● · · · ●
    0x22,   // 00100010 = · · ● · · · ● ·
    0x1C,   // 00011100 = · · · ● ● ● · ·
};

// Deuxième façon, plus lisible : un dessin en texte, 8 lignes de 8 caractères, '#' = LED allumée.
typedef struct {
    const char *name;
    const char *drawing[LED_MATRIX_SIZE];
} drawing_t;

static const drawing_t DRAWINGS[] = {
    { "A", {
        "........",
        "...##...",
        "..#..#..",
        "..#..#..",
        "..#..#..",
        "..####..",
        "..#..#..",
        "..#..#..",
    } },
    { "B", {
        "........",
        "..###...",
        "..#..#..",
        "..#..#..",
        "..###...",
        "..#..#..",
        "..#..#..",
        "..###...",
    } },
};

static void led_matrix_task(void *arg)
{
    led_matrix_t *led_matrix = (led_matrix_t *)arg;

    if (TEST_MODE) {
        ESP_LOGI(TAG, "MODE TEST : toutes les LED allumées");
        led_matrix_show(led_matrix, ALL_ON);
        vTaskDelete(NULL); // Le balayage continue en arrière-plan : la matrice reste allumée
    }

    while (1) {
        ESP_LOGI(TAG, "VISAGE (octets)");
        led_matrix_show(led_matrix, SMILING_FACE);
        vTaskDelay(pdMS_TO_TICKS(PATTERN_DELAY_MS));

        for (size_t i = 0; i < sizeof(DRAWINGS) / sizeof(DRAWINGS[0]); i++) {
            ESP_LOGI(TAG, "%s (dessin)", DRAWINGS[i].name);
            led_matrix_show_drawing(led_matrix, DRAWINGS[i].drawing);
            vTaskDelay(pdMS_TO_TICKS(PATTERN_DELAY_MS));
        }
    }
}

void start_demo_16_led_matrix(void)
{
    static led_matrix_t led_matrix;
    led_matrix = led_matrix_init(LED_MATRIX_GPIO_DATA, LED_MATRIX_GPIO_CLOCK, LED_MATRIX_GPIO_LATCH);
    led_matrix_start(&led_matrix); // Le balayage tourne en arrière-plan (esp_timer), la tâche ne fait que changer de motif

    xTaskCreate(led_matrix_task, "16-led_matrix_task", 2048, &led_matrix, 1, NULL);
}
