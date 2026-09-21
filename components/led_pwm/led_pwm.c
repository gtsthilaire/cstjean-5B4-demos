#include "led_pwm.h"
#include "esp_log.h"

static const char *TAG = "components/led_pwm";

// Mode haute vitesse = mise à jour immédiate du PWM (rapide et précis), mode basse vitesse = mise à jour avec léger retard
// mais plus économique en ressources, suffisant pour des usages simples.
#define LEDC_MODE LEDC_HIGH_SPEED_MODE

// Numéro du timer LEDC utilisé (plusieurs timers disponibles, on choisit lequel associer au canal)
// Un timer détermine la fréquence et la résolution du signal PWM.
// Plusieurs canaux peuvent partager le même timer. Ils auront alors la même fréquence,
// mais chacun peut avoir un duty cycle (rapport cyclique) différent.
#define LEDC_TIMER_NUM LEDC_TIMER_0

// Fréquence du signal PWM (en Hz) : nombre de cycles par seconde.
// - Plus elle est basse : on peut percevoir un scintillement (surtout avec des LED).
// - Plus elle est haute : la LED paraît "continue" à l'œil humain, et les moteurs tournent plus "fluide".
// Exemples :
//   - ~200 Hz : souvent suffisant pour un ventilateur lent.
//   - ~1 kHz  : ok pour des LED, mais peut encore scintiller en vidéo (c'est la valeur utilisée dans le tutoriel).
//   - 5 kHz+  : idéal pour LED visibles sans scintillement, moteurs plus silencieux.
#define LEDC_FREQ_HZ 5000

led_pwm_t led_pwm_init(gpio_num_t gpio, ledc_channel_t channel)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en sortie PWM (canal %d)", gpio, channel);

    // Configuration du timer LEDC
    // Le timer détermine la fréquence et la résolution du signal PWM.
    //
    // La résolution du duty cycle (nombre de bits) : plus il y a de bits, plus le PWM est précis.
    // Exemple :
    //   - 8 bits  : duty de 0 à 255 (256 niveaux possibles)
    //   - 10 bits : duty de 0 à 1023 (1024 niveaux possibles)
    // La résolution détermine le nombre d'échelons entre 0% et 100% du rapport cyclique.
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = (ledc_timer_bit_t)LED_PWM_DUTY_BITS,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config);

    // Configuration du canal LEDC
    // Un canal correspond à une sortie PWM liée à un GPIO spécifique.
    // Chaque canal est associé à un timer (qui fixe fréquence + résolution).
    // Sur ESP32, il y a 16 canaux disponibles (8 en mode haute vitesse et 8 en mode basse vitesse).
    ledc_channel_config_t channel_config = {
        .gpio_num = gpio,
        .speed_mode = LEDC_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0, // démarre éteinte
    };
    ledc_channel_config(&channel_config);

    return (led_pwm_t){ .gpio = gpio, .channel = channel };
}

void led_pwm_set_duty(led_pwm_t *led_pwm, uint32_t duty)
{
    if (led_pwm == NULL) {
        return;
    }

    if (duty > LED_PWM_DUTY_MAX) {
        duty = LED_PWM_DUTY_MAX;
    }

    ledc_set_duty(LEDC_MODE, led_pwm->channel, duty); // On définit le duty
    ledc_update_duty(LEDC_MODE, led_pwm->channel);    // On applique le duty
}
