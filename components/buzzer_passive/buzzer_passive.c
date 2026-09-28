/**
 * Contrairement au buzzer actif, un buzzer passif n'a pas d'oscillateur interne : il faut lui envoyer un signal
 * carré, et c'est la fréquence de ce signal qui détermine la note entendue (ex : 440 Hz = La).
 *
 * On utilise donc le PWM (périphérique LEDC).
 *
 * Dans le buzzer, une petite plaque de métal est tirée quand le GPIO est à HIGH et revient quand il est à LOW.
 * Chaque aller-retour fait une vibration, et la note dépend du nombre de vibrations par seconde (la fréquence).
 *   - Pour changer la note, on change donc la fréquence du PWM.
 *   - Le rapport cyclique (duty) ne change pas la note. On le laisse à 50 % : autant de temps tirée que relâchée,
 *     c'est ce qui fait vibrer la plaque le plus (son le plus fort et le plus net).
 * C'est l'inverse de la LED : pour elle, on change le duty (la luminosité) et la fréquence reste fixe.
 *
 * Voir components/led_pwm pour une explication plus détaillée sur le PWM.
 *
 * Attention : la fréquence est une propriété du timer (et non du canal). Tous les canaux associés au même timer
 * ont donc la même fréquence. Ce composant utilise son propre timer (LEDC_TIMER_1, mode basse vitesse) pour ne pas
 * modifier la fréquence des LED PWM (components/led_pwm utilise LEDC_TIMER_0 en mode haute vitesse).
 */

#include "buzzer_passive.h"
#include "esp_log.h"

static const char *TAG = "components/buzzer_passive";

#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER_NUM LEDC_TIMER_1

// Résolution : 10 bits (au lieu de 8).
//
// À chaque cycle du signal, le PWM compte de 0 jusqu'à une valeur maximale :
//   - 8 bits  : il compte jusqu'à 255
//   - 10 bits : il compte jusqu'à 1023
// Compter plus loin prend plus de temps, donc chaque cycle peut être plus long.
// Et un cycle plus long, c'est un son plus grave.
//
// Résultat :
//   - 8 bits  : impossible de descendre sous ~305 Hz (le Do grave, 262 Hz, ne jouerait pas)
//   - 10 bits : on descend jusqu'à ~76 Hz (toutes les notes courantes fonctionnent)
//
// Pour une LED, les bits servent à régler finement la luminosité. Le buzzer, lui, reste toujours à 50 %,
// donc on peut en mettre plus sans rien perdre.
#define LEDC_DUTY_BITS 10
#define LEDC_DUTY_HALF ((1 << LEDC_DUTY_BITS) / 2)  // 50 % → signal carré
#define LEDC_INITIAL_FREQ_HZ 2000                   // Fréquence de départ (modifiée ensuite par buzzer_passive_tone)

buzzer_passive_t buzzer_passive_init(gpio_num_t gpio, ledc_channel_t channel)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie PWM (canal %d)", gpio, channel);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = (ledc_timer_bit_t)LEDC_DUTY_BITS,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_INITIAL_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = gpio,
        .speed_mode = LEDC_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0, // démarre silencieux
    };
    ledc_channel_config(&channel_config);

    return (buzzer_passive_t){ .gpio = gpio, .channel = channel };
}

void buzzer_passive_tone(buzzer_passive_t *buzzer, uint32_t freq_hz)
{
    if (buzzer == NULL) {
        return;
    }

    if (freq_hz == 0) {
        buzzer_passive_stop(buzzer); // 0 Hz = silence (pratique pour les pauses d'une mélodie)
        return;
    }

    // On change la note. Hors de la plage possible (voir LEDC_DUTY_BITS), le timer garde l'ancienne fréquence.
    if (ledc_set_freq(LEDC_MODE, LEDC_TIMER_NUM, freq_hz) != ESP_OK) {
        ESP_LOGW(TAG, "Fréquence de %d Hz impossible (plage : ~76 Hz à ~78 kHz)", (int)freq_hz);
        return;
    }

    ledc_set_duty(LEDC_MODE, buzzer->channel, LEDC_DUTY_HALF);  // Signal carré (50 %)
    ledc_update_duty(LEDC_MODE, buzzer->channel);
}

void buzzer_passive_stop(buzzer_passive_t *buzzer)
{
    if (buzzer == NULL) {
        return;
    }

    ledc_set_duty(LEDC_MODE, buzzer->channel, 0);
    ledc_update_duty(LEDC_MODE, buzzer->channel);
}
