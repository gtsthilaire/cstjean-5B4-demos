/**
 * Un moteur DC tourne dans un sens ou dans l'autre selon le sens du courant qui le traverse :
 * si on inverse ses deux fils sur la pile, il tourne à l'envers. Avec un relais (voir components/relay),
 * on peut seulement l'allumer ou l'éteindre. Pour changer de sens, il faudrait inverser les fils à la main.
 *
 * Le pont en H fait cette inversion électroniquement. C'est 4 interrupteurs placés autour du moteur,
 * ce qui dessine la lettre H (le moteur est la barre du milieu) :
 *
 *        +V           +V
 *        |            |
 *       [A]          [B]
 *        |            |
 *        +---( M )----+
 *        |            |
 *       [C]          [D]
 *        |            |
 *       GND          GND
 *
 *   - A et D fermés : le courant traverse le moteur de gauche à droite → il tourne dans un sens.
 *   - B et C fermés : le courant traverse le moteur de droite à gauche → il tourne dans l'autre sens.
 *   - A et C fermés en même temps (ou B et D) : court-circuit entre +V et GND ! Le L293D l'empêche pour nous.
 *
 * Le L293D contient deux ponts en H (on n'en utilise qu'un) et s'occupe de fermer les bons interrupteurs.
 * On lui donne seulement le sens voulu, avec les broches IN1 et IN2 :
 *       IN1 = 1, IN2 = 0 → un sens
 *       IN1 = 0, IN2 = 1 → l'autre sens
 *       IN1 = IN2        → arrêt (les deux bornes du moteur sont au même niveau, aucun courant ne passe)
 *
 * La vitesse se règle avec la broche EN (enable) : on y envoie un signal PWM. Le pont est alors fermé et ouvert
 * très rapidement. Plus le rapport cyclique est grand, plus le moteur reçoit de puissance et plus il tourne vite.
 * C'est le même principe que la luminosité d'une LED (voir components/led_pwm pour plus de détails sur le PWM).
 *
 * Comme le relais, le L293D laisse le moteur utiliser sa propre alimentation : le GPIO ne fait que commander.
 *
 * Ce composant utilise son propre timer LEDC (LEDC_TIMER_1, mode haute vitesse) pour ne pas modifier la fréquence
 * des LED PWM (components/led_pwm utilise LEDC_TIMER_0 en mode haute vitesse).
 */

#include "dc_motor.h"
#include "esp_log.h"

static const char *TAG = "components/dc_motor";

#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_TIMER_NUM LEDC_TIMER_1
#define LEDC_DUTY_BITS 8                          // Résolution : 8 bits → duty de 0 à 255
#define LEDC_DUTY_MAX ((1 << LEDC_DUTY_BITS) - 1) // 255 pour 8 bits
#define LEDC_FREQ_HZ 1000                         // 1 kHz, comme dans le tutoriel

static void set_output(gpio_num_t gpio)
{
    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(gpio, 0);
}

dc_motor_t dc_motor_init(gpio_num_t gpio_in1, gpio_num_t gpio_in2, gpio_num_t gpio_enable, ledc_channel_t channel)
{
    ESP_LOGI(TAG, "Configuration des GPIO %d et %d (sens) et du GPIO %d en sortie PWM (canal %d)",
             gpio_in1, gpio_in2, gpio_enable, channel);

    set_output(gpio_in1);
    set_output(gpio_in2);

    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = (ledc_timer_bit_t)LEDC_DUTY_BITS,
        .timer_num = LEDC_TIMER_NUM,
        .freq_hz = LEDC_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_config);

    ledc_channel_config_t channel_config = {
        .gpio_num = gpio_enable,
        .speed_mode = LEDC_MODE,
        .channel = channel,
        .timer_sel = LEDC_TIMER_NUM,
        .duty = 0, // démarre arrêté
    };
    ledc_channel_config(&channel_config);

    return (dc_motor_t){ .gpio_in1 = gpio_in1, .gpio_in2 = gpio_in2, .channel = channel };
}

void dc_motor_set_speed(dc_motor_t *dc_motor, int speed)
{
    if (dc_motor == NULL) {
        return;
    }

    if (speed > DC_MOTOR_SPEED_MAX) speed = DC_MOTOR_SPEED_MAX;
    if (speed < -DC_MOTOR_SPEED_MAX) speed = -DC_MOTOR_SPEED_MAX;

    // Le signe choisit le sens
    gpio_set_level(dc_motor->gpio_in1, speed > 0);
    gpio_set_level(dc_motor->gpio_in2, speed < 0);

    // La valeur (sans le signe) choisit la vitesse : % → duty
    int percent = speed < 0 ? -speed : speed;
    int duty = (percent * LEDC_DUTY_MAX) / DC_MOTOR_SPEED_MAX;

    ledc_set_duty(LEDC_MODE, dc_motor->channel, duty);
    ledc_update_duty(LEDC_MODE, dc_motor->channel);
}
