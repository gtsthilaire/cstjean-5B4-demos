/**
 * Notre moteur pas-à-pas (le 28BYJ-48 du kit) contient 4 bobines (des électroaimants) placées autour d'un aimant
 * qui tourne (le rotor). Quand on alimente une bobine, le rotor est attiré vers elle et s'y arrête. En alimentant
 * les bobines l'une après l'autre, on fait avancer le rotor d'un petit angle à la fois : un « pas ».
 *
 *   Sens 1 : bobine A → B → C → D → A → ...
 *   Sens 2 : bobine D → C → B → A → D → ...
 *
 * D'autres moteurs pas-à-pas existent : nombre de bobines différent, rotor en fer plutôt qu'aimanté, etc.
 * Mais le principe reste le même : on alimente les bobines dans le bon ordre, et le moteur avance d'un pas à la fois.
 *
 * Pourquoi 2048 pas pour un tour ?
 *   - Le rotor a plusieurs pôles (pensez à une roue dentée aimantée) : la séquence A → B → C → D ne le fait pas
 *     tourner d'un tour, mais seulement d'un petit bout. Il faut répéter la séquence 8 fois, donc 32 pas,
 *     pour que le rotor fasse un tour complet.
 *   - Ensuite, des engrenages à l'intérieur du moteur réduisent la vitesse : le rotor doit faire 64 tours
 *     pour que l'axe de sortie (celui qu'on voit) en fasse un seul.
 *   - Au total : 32 pas × 64 = 2048 pas pour un tour de l'axe de sortie. Un pas = un angle minuscule (moins de 0.2°),
 *     mais en échange l'axe tourne avec beaucoup plus de force et beaucoup de précision.
 *
 * On sait donc exactement de combien le moteur a tourné : il suffit de compter les pas.
 * La vitesse dépend du temps d'attente entre deux pas. Trop court, le moteur n'a pas le temps de suivre et il « saute ».
 *
 * Le module ULN2003 : un GPIO ne fournit pas assez de courant pour alimenter une bobine. Le ULN2003 contient
 * 4 interrupteurs électroniques (des transistors), un par bobine : quand un GPIO est à l'état haut, l'interrupteur
 * correspondant se ferme et la bobine reçoit le courant de l'alimentation 5V. C'est le même rôle que le transistor
 * qui commande le relais (démo 17-relay), mais en 4 exemplaires dans une seule puce.
 * Les 4 LED du module (A, B, C, D) s'allument quand leur bobine est alimentée.
 *
 * Comparaison avec les autres moteurs du kit :
 *   - moteur DC (components/dc_motor) : on choisit la vitesse, mais on ne sait pas où il est rendu ;
 *   - servo (components/servo) : on choisit un angle précis, mais seulement entre 0 et 180 degrés ;
 *   - pas-à-pas : on choisit un nombre de pas précis, et il peut faire autant de tours qu'on veut.
 */

#include "stepper.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "components/stepper";

#define STEPPER_DELAY_MS_MIN 3 // En dessous, le moteur n'a pas le temps de suivre

// Alimente seulement la bobine actuelle, éteint les autres.
static void apply_coil(stepper_t *stepper)
{
    for (int i = 0; i < STEPPER_COILS; i++) {
        gpio_set_level(stepper->gpios[i], i == stepper->coil);
    }
}

stepper_t stepper_init(gpio_num_t gpio_in1, gpio_num_t gpio_in2, gpio_num_t gpio_in3, gpio_num_t gpio_in4)
{
    ESP_LOGI(TAG, "Configuration des GPIO %d, %d, %d et %d en sortie", gpio_in1, gpio_in2, gpio_in3, gpio_in4);

    stepper_t stepper = { .gpios = { gpio_in1, gpio_in2, gpio_in3, gpio_in4 }, .coil = 0 };

    for (int i = 0; i < STEPPER_COILS; i++) {
        gpio_reset_pin(stepper.gpios[i]);
        gpio_set_direction(stepper.gpios[i], GPIO_MODE_OUTPUT);
        gpio_set_level(stepper.gpios[i], 0);
    }

    return stepper;
}

void stepper_move(stepper_t *stepper, int steps, int delay_ms)
{
    if (stepper == NULL) {
        return;
    }

    if (delay_ms < STEPPER_DELAY_MS_MIN) delay_ms = STEPPER_DELAY_MS_MIN;

    int direction = steps > 0 ? 1 : -1;
    int count = steps > 0 ? steps : -steps;

    for (int i = 0; i < count; i++) {
        // Passe à la bobine suivante (ou précédente), en revenant au début après la dernière
        stepper->coil = (stepper->coil + direction + STEPPER_COILS) % STEPPER_COILS;
        apply_coil(stepper);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }

    // Éteint toutes les bobines à la fin : le moteur ne chauffe pas inutilement
    for (int i = 0; i < STEPPER_COILS; i++) {
        gpio_set_level(stepper->gpios[i], 0);
    }
}
