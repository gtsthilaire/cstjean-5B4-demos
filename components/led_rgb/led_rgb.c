/**
 * Démonstration de l'utilisation d'une LED RGB.
 * La LED RGB peut afficher différentes couleurs en combinant les lumières rouge, verte et bleue.
 *
 * ** Voir le component led_pwm pour une explication plus détaillée sur le PWM. **
 *
 * Tutoriel original : Chapitre 5 - RGB LED (Freenove)
 */

#include "led_rgb.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_MODE       LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER_NUM  LEDC_TIMER_0
#define LEDC_DUTY_RES   LEDC_TIMER_8_BIT            // Résolution : 8 bits → duty de 0 à 255
#define LEDC_FREQ_HZ    1000                        // Fréquence du PWM en Hz (ici 1 kHz)
#define LEDC_DUTY_MAX   ((1 << LEDC_DUTY_RES) - 1)  // 255 pour 8 bits

#define LEDC_CHANNEL_R    LEDC_CHANNEL_0
#define LEDC_CHANNEL_G    LEDC_CHANNEL_1
#define LEDC_CHANNEL_B    LEDC_CHANNEL_2

static const char *TAG = "led_rgb.c";

static void led_rgb_init(gpio_num_t gpio_r, gpio_num_t gpio_g, gpio_num_t gpio_b)
{
    ledc_timer_config_t timer_config = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_NUM,
        .freq_hz          = LEDC_FREQ_HZ,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t ch_config_r = {
        .gpio_num   = gpio_r,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL_R,
        .timer_sel  = LEDC_TIMER_NUM,
        .duty       = 0,
    };
    ledc_channel_config(&ch_config_r);

    ledc_channel_config_t ch_config_g = {
        .gpio_num   = gpio_g,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL_G,
        .timer_sel  = LEDC_TIMER_NUM,
        .duty       = 0,
    };
    ledc_channel_config(&ch_config_g);

    ledc_channel_config_t ch_config_b = {
        .gpio_num   = gpio_b,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL_B,
        .timer_sel  = LEDC_TIMER_NUM,
        .duty       = 0,
    };
    ledc_channel_config(&ch_config_b);
}

static void led_rgb_set(uint8_t r, uint8_t g, uint8_t b)
{
    // Inversion de la valeur car la LED est active à LOW
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_R, LEDC_DUTY_MAX - r);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_G, LEDC_DUTY_MAX - g);
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_B, LEDC_DUTY_MAX - b);

    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_R);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_G);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_B);
}

static void led_rgb_task(void *arg)
{
    while (1) {
        ESP_LOGD(TAG, "Led RED");
        led_rgb_set(LEDC_DUTY_MAX, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGD(TAG, "Led GREEN");
        led_rgb_set(0, LEDC_DUTY_MAX, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGD(TAG, "Led BLUE");
        led_rgb_set(0, 0, LEDC_DUTY_MAX);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGD(TAG, "Led WHITE");
        led_rgb_set(LEDC_DUTY_MAX, LEDC_DUTY_MAX, LEDC_DUTY_MAX);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_led_rgb_task(gpio_num_t gpio_r, gpio_num_t gpio_g, gpio_num_t gpio_b)
{
    led_rgb_init(gpio_r, gpio_g, gpio_b);

    xTaskCreate(led_rgb_task, "led_rgb_task", 2048, NULL, 5, NULL);
}