#include "servo.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/18-servo";

#define SERVO_GPIO GPIO_NUM_15
#define SERVO_CHANNEL LEDC_CHANNEL_1

#define SERVO_STEP_DEG 5

static void servo_task(void *arg)
{
    servo_t *servo = (servo_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "0 → %d degrés", SERVO_ANGLE_MAX);
        for (int angle = 0; angle <= SERVO_ANGLE_MAX; angle += SERVO_STEP_DEG) {
            servo_set_angle(servo, angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "%d → 0 degré", SERVO_ANGLE_MAX);
        for (int angle = SERVO_ANGLE_MAX; angle >= 0; angle -= SERVO_STEP_DEG) {
            servo_set_angle(servo, angle);
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_18_servo(void)
{
    static servo_t servo;
    servo = servo_init(SERVO_GPIO, SERVO_CHANNEL);

    xTaskCreate(servo_task, "18-servo_task", 2048, &servo, 1, NULL);
}
