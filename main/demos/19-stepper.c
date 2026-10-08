#include "stepper.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/19-stepper";

#define STEPPER_GPIO_IN1 GPIO_NUM_14
#define STEPPER_GPIO_IN2 GPIO_NUM_27
#define STEPPER_GPIO_IN3 GPIO_NUM_26
#define STEPPER_GPIO_IN4 GPIO_NUM_25

// Temps entre deux pas : 2048 pas × 5 ms ≈ 10 secondes par tour.
// Plus court, le moteur risque de « sauter » des pas (il n'a pas le temps de suivre) et de ne pas faire un tour complet.
#define STEPPER_DELAY_MS 5

#define STEPPER_STEPS_PER_QUARTER (STEPPER_STEPS_PER_TURN / 4) // 512 pas pour 90 degrés

static void stepper_task(void *arg)
{
    stepper_t *stepper = (stepper_t *)arg;

    while (1) {
        // Un tour en 4 quarts de tour : le moteur s'arrête exactement à 90, 180, 270 puis 360 degrés.
        for (int quarter = 1; quarter <= 4; quarter++) {
            ESP_LOGI(TAG, "Quart de tour %d/4 (%d pas)", quarter, STEPPER_STEPS_PER_QUARTER);
            stepper_move(stepper, STEPPER_STEPS_PER_QUARTER, STEPPER_DELAY_MS);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void start_demo_19_stepper(void)
{
    static stepper_t stepper;
    stepper = stepper_init(STEPPER_GPIO_IN1, STEPPER_GPIO_IN2, STEPPER_GPIO_IN3, STEPPER_GPIO_IN4);

    xTaskCreate(stepper_task, "19-stepper_task", 2048, &stepper, 1, NULL);
}
