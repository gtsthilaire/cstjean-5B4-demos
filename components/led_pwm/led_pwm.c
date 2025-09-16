/**
 * Démonstration de l'utilisation d'une LED avec PWM.
 *
 * Tutoriel original : Chapitre 4 - Analog and PWM (Freenove)
 * 
 * Pour nos GPIO, une sortie numérique est soit haute (3.3V) soit basse (0V).
 * 
 * Le PWM (Pulse Width Modulation) est une technique qui permet de simuler une sortie analogique en faisant varier
 * la largeur d'une impulsion numérique. En variant le rapport cyclique (duty cycle) de l'impulsion, on peut contrôler
 * la puissance moyenne délivrée à une charge, comme une LED.
 * 
 * Sur une période de temps fixe, une impulsion PWM alterne entre un état haut (3.3V) et un état bas (0V). Le rapport cyclique
 * est défini comme le pourcentage de temps pendant lequel l'impulsion est haute par rapport à la période totale. Par exemple,
 * un rapport cyclique de 50% signifie que l'impulsion est haute pendant la moitié du temps et basse pendant l'autre moitié.
 * On aura donc une puissance moyenne de 50% de la puissance maximale.
 * 
 * Le PWM est souvent utilisé pour :
 * - Le contrôle de la luminosité des LED
 * - Le contrôle de la vitesse des moteurs
 * - La génération de signaux audio
 * - Etc.
 */

#include "led_pwm.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

// Mode haute vitesse = mise à jour immédiate du PWM (rapide et précis), mode basse vitesse = mise à jour avec léger retard 
// mais plus économique en ressources, suffisant pour des usages simples.
#define LEDC_MODE       LEDC_HIGH_SPEED_MODE

// Numéro du timer LEDC utilisé (plusieurs timers disponibles, on choisit lequel associer au canal)
// Un timer détermine la fréquence et la résolution du signal PWM. 
// Plusieurs canaux peuvent partager le même timer. Ils auront alors la même fréquence,
// mais chacun peut avoir un duty cycle (rapport cyclique) différent.
#define LEDC_TIMER_NUM  LEDC_TIMER_0

// Un canal correspond à une sortie PWM liée à un GPIO spécifique.
// Chaque canal est associé à un timer (qui fixe fréquence + résolution).
// Plusieurs canaux peuvent partager un même timer : même fréquence mais duty cycle indépendant.
// Sur ESP32, il y a 16 canaux disponibles.
#define LEDC_CHANNEL    LEDC_CHANNEL_0

// Résolution du duty cycle (nombre de bits) : plus il y a de bits, plus le PWM est précis.
// Exemple :
//   - 8 bits  : duty de 0 à 255 (256 niveaux possibles)
//   - 10 bits : duty de 0 à 1023 (1024 niveaux possibles)
// La résolution détermine le nombre d’échelons entre 0% et 100% du rapport cyclique.
#define LEDC_DUTY_RES   LEDC_TIMER_8_BIT

// Fréquence du signal PWM (en Hz) : nombre de cycles par seconde.
// - Plus elle est basse : on peut percevoir un scintillement (surtout avec des LED).
// - Plus elle est haute : la LED paraît "continue" à l’œil humain, et les moteurs tournent plus "fluide".
// Exemples :
//   - ~200 Hz : souvent suffisant pour un ventilateur lent.
//   - ~1 kHz  : ok pour des LED, mais peut encore scintiller en vidéo.
//   - 5 kHz+  : idéal pour LED visibles sans scintillement, moteurs plus silencieux.
#define LEDC_FREQ_HZ    5000

// Valeur maximale du duty cycle possible pour la résolution choisie.
// Calcul : (2^résolution) - 1
#define LEDC_DUTY_MAX   ((1 << LEDC_DUTY_RES) - 1)  // 255 pour 8 bits

static const char *TAG = "led_pwm.c";

static void led_pwm_init(gpio_num_t gpio)
{
    // Configuration du timer LEDC
    // Le timer détermine la fréquence et la résolution du signal PWM.
    ledc_timer_config_t timer_config = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_NUM,
        .freq_hz          = LEDC_FREQ_HZ,
    };
    ledc_timer_config(&timer_config);

    // Configuration du canal LEDC
    // Le canal est associé à un GPIO spécifique et à un timer.
    ledc_channel_config_t ch_config = {
        .gpio_num   = gpio,
        .speed_mode = LEDC_MODE,
        .channel    = LEDC_CHANNEL,
        .timer_sel  = LEDC_TIMER_NUM,
        .duty       = 0,   // démarre éteint
    };

    ledc_channel_config(&ch_config);
}

static void led_pwm_task(void *arg)
{
    const int step = LEDC_DUTY_MAX / 10; // pour faire des pas de 10% sinon c'est trop lent

    while (1) {
        ESP_LOGI(TAG, "Led FADE IN");
        for (int duty = 0; duty <= LEDC_DUTY_MAX; duty += step) {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty); // On définit le duty
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL); // On applique le duty
            vTaskDelay(10);
        }

        ESP_LOGI(TAG, "Led FADE OUT");
        for (int duty = LEDC_DUTY_MAX; duty >= 0; duty -= step) {
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(10);
        }
    }
}

void start_demo_led_pwm_task(gpio_num_t gpio)
{
    led_pwm_init(gpio);
    xTaskCreate(led_pwm_task, "led_pwm_task", 2048, NULL, 5, NULL);
}