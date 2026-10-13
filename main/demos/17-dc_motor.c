#include "dc_motor.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/17-dc_motor";

// Broches du tutoriel. Le GPIO 12 sert aussi au démarrage de l'ESP32 : si la carte ne démarre pas pendant
// qu'elle est branchée au circuit, débrancher le fil du GPIO 12 le temps du flash.
#define MOTOR_GPIO_IN1 GPIO_NUM_12
#define MOTOR_GPIO_IN2 GPIO_NUM_14
#define MOTOR_GPIO_ENABLE GPIO_NUM_13
#define MOTOR_CHANNEL LEDC_CHANNEL_0

#define MOTOR_STEP 5 // Changement de vitesse à chaque étape (%)

// Fait passer la vitesse de 0 à max, puis de max à 0. direction = 1 ou -1 pour le sens de rotation.
static void ramp(dc_motor_t *motor, int direction)
{
    for (int speed = 0; speed <= DC_MOTOR_SPEED_MAX; speed += MOTOR_STEP) {
        dc_motor_set_speed(motor, direction * speed);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    for (int speed = DC_MOTOR_SPEED_MAX; speed >= 0; speed -= MOTOR_STEP) {
        dc_motor_set_speed(motor, direction * speed);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void dc_motor_task(void *arg)
{
    dc_motor_t *motor = (dc_motor_t *)arg;

    while (1) {
        ESP_LOGI(TAG, "Sens 1 : accélère puis ralentit");
        ramp(motor, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));

        ESP_LOGI(TAG, "Sens 2 : accélère puis ralentit");
        ramp(motor, -1);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_17_dc_motor(void)
{
    static dc_motor_t motor;
    motor = dc_motor_init(MOTOR_GPIO_IN1, MOTOR_GPIO_IN2, MOTOR_GPIO_ENABLE, MOTOR_CHANNEL);

    xTaskCreate(dc_motor_task, "17-dc_motor_task", 4096, &motor, 1, NULL);
}
