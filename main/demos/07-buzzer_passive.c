#include "buzzer_passive.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char *TAG = "demos/07-buzzer_passive";

#define BUZZER_GPIO GPIO_NUM_13
#define BUZZER_CHANNEL LEDC_CHANNEL_0

#define SIREN_CENTER_HZ 2000 // Fréquence centrale de la sirène
#define SIREN_AMPLITUDE_HZ 500 // Variation autour de la fréquence centrale
#define SIREN_STEP_DEG 10 // Incrément de l'angle de la sinusoïde à chaque pas

static void buzzer_passive_task(void *arg)
{
    buzzer_passive_t *buzzer = (buzzer_passive_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "Sirène");

        // Un tour complet de sinusoïde (0 à 360 degrés) : la fréquence monte, redescend, puis revient au centre.
        for (int deg = 0; deg < 360; deg += SIREN_STEP_DEG) {
            float rad = deg * (M_PI / 180.0f);
            uint32_t freq_hz = SIREN_CENTER_HZ + (int)(sinf(rad) * SIREN_AMPLITUDE_HZ);
            buzzer_passive_tone(buzzer, freq_hz);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void start_demo_07_buzzer_passive(void)
{
    static buzzer_passive_t buzzer;
    buzzer = buzzer_passive_init(BUZZER_GPIO, BUZZER_CHANNEL);

    xTaskCreate(buzzer_passive_task, "07-buzzer_passive_task", 2048, &buzzer, 1, NULL);
}
