/**
 * Démonstration de l'utilisation d'un touch sensor simple.
 * Le capteur (le fil) est surveillé et son état est affiché dans la console.
 *
 * Sur l'ESP32, les broches tactiles disponibles sont : 
 * GPIO4 (T0), GPIO0 (T1), GPIO2 (T2), GPIO15 (T3), GPIO13 (T4), GPIO12 (T5), 
 * GPIO14 (T6), GPIO27 (T7), GPIO33 (T8) et GPIO32 (T9).
 *
 * Tutoriel original : Chapitre 10 - Touch sensor (Freenove)
 */

#include "touch.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "touch.c";

static touch_pad_t s_touch_pad = -1;

static void touch_init(touch_pad_t pad)
{
    s_touch_pad = pad;

    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);   // Les mesures sont déclenchées automatiquement par un timer matériel interne
    touch_pad_config(s_touch_pad, 0);               // Configurer la broche tactile
    touch_pad_filter_start(10);                     // Active le filtre logiciel. Met à jour la valeur toutes les 10 ms pour réduire le bruit.
}

static void touch_task(void *arg)
{
    while (1) {
        uint16_t raw = 0, filt = 0;

        touch_pad_read_raw_data(s_touch_pad, &raw);
        touch_pad_read_filtered(s_touch_pad, &filt);

        ESP_LOGD(TAG, "Touch raw=%5u  filtered=%5u", raw, filt);

        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

void start_demo_touch_task(touch_pad_t pad)
{
    touch_init(pad);
    
    xTaskCreate(touch_task, "touch_task", 2048, NULL, 5, NULL);
}