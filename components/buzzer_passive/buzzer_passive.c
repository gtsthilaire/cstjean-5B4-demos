/**
 * Démonstration de l'utilisation d'un buzzer passif.
 * Le buzzer émet un son toutes les X millisecondes.
 * Contrairement au buzzer actif, le buzzer passif nécessite une modulation de fréquence (PWM) pour émettre un son.
 *
 * ** Voir le component led_pwm pour une explication plus détaillée sur le PWM. **
 *
 * Tutoriel original : Chapitre 7 - Buzzer (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de gestion de bouton pour activer/désactiver le buzzer.
 */

#include "buzzer_passive.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_NUM LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_8_BIT           // Résolution : 8 bits → duty de 0 à 255
#define LEDC_FREQ_HZ 2000                        // Fréquence du PWM en Hz (ici 2000 Hz)
#define LEDC_DUTY_MAX ((1 << LEDC_DUTY_RES) - 1) // 255 pour 8 bits
#define LEDC_DUTY_ON (LEDC_DUTY_MAX / 2)         // ~50%

#define AMP_FREQ 500 // amplitude de variation (Hz)
#define STEP_DEG 10  // incrément en degrés

static const char *TAG = "buzzer_passive.c";

static void buzzer_passive_init(gpio_num_t buzzer_gpio)
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_FREQ_HZ,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t ch_config = {
        .gpio_num = buzzer_gpio,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0, // démarre éteint
    };
    ledc_channel_config(&ch_config);
}

static void buzzer_passive_task(void *arg)
{
    while (1) {
        ESP_LOGI(TAG, "Buzzer ON");
        
        // Fait varier la fréquence du PWM selon une sinusoïde pour créer un effet de sirène.
        for (int x = 0; x < 360; x += STEP_DEG) {
            float rad = x * (M_PI / 180.0f);
            float sinVal = sinf(rad);
            int toneVal = LEDC_FREQ_HZ + (int)(sinVal * AMP_FREQ);
            if (toneVal < 1)
                toneVal = 1;
            ledc_set_freq(LEDC_MODE, LEDC_TIMER_NUM, toneVal);
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY_ON);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

void start_demo_buzzer_passive_task(gpio_num_t buzzer_gpio)
{
    buzzer_passive_init(buzzer_gpio);

    xTaskCreate(buzzer_passive_task, "buzzer_passive_task", 2048, NULL, 5, NULL);
}