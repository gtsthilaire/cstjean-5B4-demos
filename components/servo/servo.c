/**
 * Un servo moteur est un moteur qui se positionne à un angle précis, entre 0 et 180 degrés.
 * L'angle est contrôlé par un signal PWM de 50 Hz (une période de 20 ms) : c'est la durée de l'impulsion
 * à l'état haut qui détermine l'angle.
 *   - 0.5 ms → 0 degré
 *   - 1.5 ms → 90 degrés
 *   - 2.5 ms → 180 degrés
 *
 * Voir components/led_pwm pour une explication plus détaillée sur le PWM.
 *
 * Attention : le servo moteur doit être alimenté par une source externe de 5V (il consomme trop pour le 3.3V de l'ESP32).
 *
 * Ce composant utilise son propre timer LEDC (LEDC_TIMER_2, mode basse vitesse), car sa fréquence (50 Hz) est
 * différente de celle des LED PWM et du buzzer passif.
 */

#include "servo.h"
#include "esp_log.h"

static const char *TAG = "components/servo";

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_NUM LEDC_TIMER_2
#define LEDC_DUTY_BITS 13                            // Résolution : 13 bits → duty de 0 à 8191 (plus précis qu'en 8 bits)
#define LEDC_DUTY_MAX ((1 << LEDC_DUTY_BITS) - 1)    // 8191 pour 13 bits
#define LEDC_FREQ_HZ 50                              // 50 Hz, soit une période de 20 ms
#define LEDC_PERIOD_US (1000000 / LEDC_FREQ_HZ)      // Période en microsecondes (20000 µs)

#define SERVO_PULSE_US_MIN 500  // Durée de l'impulsion pour 0 degré (0.5 ms)
#define SERVO_PULSE_US_MAX 2500 // Durée de l'impulsion pour 180 degrés (2.5 ms)

servo_t servo_init(gpio_num_t gpio, ledc_channel_t channel)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie PWM 50 Hz (canal %d)", gpio, channel);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = (ledc_timer_bit_t)LEDC_DUTY_BITS,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = gpio,
        .speed_mode = LEDC_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0, // pas d'impulsion : le servo ne bouge pas tant qu'on ne lui donne pas d'angle
    };
    ledc_channel_config(&channel_config);

    return (servo_t){ .gpio = gpio, .channel = channel };
}

void servo_set_angle(servo_t *servo, int angle)
{
    if (servo == NULL) {
        return;
    }

    if (angle < 0) angle = 0;
    if (angle > SERVO_ANGLE_MAX) angle = SERVO_ANGLE_MAX;

    // Angle → durée de l'impulsion (µs) → duty (proportion de la période de 20 ms)
    int pulse_us = SERVO_PULSE_US_MIN + ((SERVO_PULSE_US_MAX - SERVO_PULSE_US_MIN) * angle) / SERVO_ANGLE_MAX;
    int duty = (pulse_us * LEDC_DUTY_MAX) / LEDC_PERIOD_US;

    ledc_set_duty(LEDC_MODE, servo->channel, duty);
    ledc_update_duty(LEDC_MODE, servo->channel);
}
