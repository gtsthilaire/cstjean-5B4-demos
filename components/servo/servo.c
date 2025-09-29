/**
 * Démonstration de l'utilisation d'un servo moteur.
 * Un servo moteur est un moteur électrique qui peut être positionné à un angle précis entre 0 et 180 degrés.
 * Le positionnement est contrôlé par un signal PWM (Pulse Width Modulation).

 * Le temps passé en valeur haute (duty cycle) détermine l'angle du servo.
 * 0.5ms (0 degré) à 2.5ms (180 degrés) dans une période de 20ms (50Hz).
 * 
 * Attention de brancher le servo moteur à une source d'alimentation externe 5V.
 * 
 * =============================================================================
 * [!] Voir le component led_pwm pour une explication plus détaillée sur le PWM.
 * =============================================================================
 *
 * Tutoriel original : Chapitre 18 - Servo (Freenove)
 */

#include "servo.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_NUM LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT                 // Résolution : 13 bits → duty de 0 à 8191
#define LEDC_FREQ_HZ 50                                 // Fréquence du PWM en Hz (ici 50 Hz). Équivaut à une période de 20ms.
#define LEDC_FREQ_PERIOD_US (1000000 / LEDC_FREQ_HZ)    // Période en microsecondes (20000us pour 50Hz)
#define LEDC_DUTY_MAX ((1 << LEDC_DUTY_RES) - 1)        // 8191 pour 13 bits

#define SERVO_PULSEWIDTH_US_MIN 500   // Durée minimale du signal en microsecondes (0.5ms)
#define SERVO_PULSEWIDTH_US_MAX 2500  // Durée maximale du signal en microsecondes (2.5ms)
#define SERVO_DEGREE_MAX 180          // Angle maximum du servo

static const char *TAG = "servo.c";

static void servo_init(gpio_num_t servo_gpio)
{
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_FREQ_HZ,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t ch_config = {
        .gpio_num = servo_gpio,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0,
    };
    ledc_channel_config(&ch_config);
}

void set_servo_angle(int angle)
{
    if (angle < 0) angle = 0;
    if (angle > SERVO_DEGREE_MAX) angle = SERVO_DEGREE_MAX;

    // Calcul du duty en fonction de l'angle
    int pulsewidth = SERVO_PULSEWIDTH_US_MIN + ((SERVO_PULSEWIDTH_US_MAX - SERVO_PULSEWIDTH_US_MIN) * angle) / SERVO_DEGREE_MAX;
    int duty = (pulsewidth * LEDC_DUTY_MAX) / LEDC_FREQ_PERIOD_US;

    ESP_LOGI(TAG, "Angle: %d degrés, Duty: %d", angle, duty);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

static void servo_task(void *arg)
{
    while (1) {        
        for (int angle = 0; angle <= SERVO_DEGREE_MAX; angle += 5) {
            set_servo_angle(angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        
        vTaskDelay(pdMS_TO_TICKS(1000));

        for (int angle = SERVO_DEGREE_MAX; angle >= 0; angle -= 5) {
            set_servo_angle(angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_servo_task(gpio_num_t servo_gpio)
{
    servo_init(servo_gpio);

    xTaskCreate(servo_task, "servo_task", 2048, NULL, 5, NULL);
}