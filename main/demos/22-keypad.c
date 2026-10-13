#include "keypad.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/22-keypad";

static const gpio_num_t KEYPAD_ROW_GPIOS[KEYPAD_ROWS] = { GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26, GPIO_NUM_25 };
static const gpio_num_t KEYPAD_COL_GPIOS[KEYPAD_COLS] = { GPIO_NUM_13, GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_23 };

static void keypad_task(void *arg)
{
    keypad_t *keypad = (keypad_t *)arg;

    while (1) {
        char key = keypad_get_key(keypad);

        if (key != 0) {
            ESP_LOGI(TAG, "Touche : %c", key);
        }

        // Balayage toutes les 20 ms : assez rapide pour ne rater aucune touche,
        // assez lent pour ignorer les rebonds (voir la démo 02)
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void start_demo_22_keypad(void)
{
    static keypad_t keypad;
    keypad = keypad_init(KEYPAD_ROW_GPIOS, KEYPAD_COL_GPIOS);

    xTaskCreate(keypad_task, "22-keypad_task", 4096, &keypad, 1, NULL);
}
