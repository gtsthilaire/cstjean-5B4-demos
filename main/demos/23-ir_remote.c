#include "ir_remote.h"

#include "esp_log.h"
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/23-ir_remote";

#define IR_REMOTE_GPIO GPIO_NUM_15

static void ir_remote_task(void *arg)
{
    ir_remote_t *ir_remote = (ir_remote_t *)arg;

    while (1) {
        uint32_t code;

        // ir_remote_read attend qu'une touche soit pressée. Pendant l'attente, elle fait déjà
        // des pauses (vTaskDelay) : pas besoin d'en ajouter une dans la boucle.
        if (ir_remote_read(ir_remote, &code)) {
            ESP_LOGI(TAG, "Code : 0x%" PRIX32, code);
        }
    }
}

void start_demo_23_ir_remote(void)
{
    static ir_remote_t ir_remote;
    ir_remote = ir_remote_init(IR_REMOTE_GPIO);

    xTaskCreate(ir_remote_task, "23-ir_remote_task", 4096, &ir_remote, 1, NULL);
}
