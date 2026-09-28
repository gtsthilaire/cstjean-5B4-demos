#include "joystick.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/14-joystick";

// Broches du tutoriel Freenove
#define JOYSTICK_GPIO_X GPIO_NUM_13 // ADC2
#define JOYSTICK_GPIO_Y GPIO_NUM_12 // ADC2 (même unité que X)
#define JOYSTICK_GPIO_Z GPIO_NUM_14

static void joystick_task(void *arg)
{
    joystick_t *joystick = (joystick_t *)arg;
    int last_x = -1000, last_y = -1000; // valeurs impossibles : force le premier affichage
    bool last_pressed = false;

    while (1) {
        int x = 0, y = 0;
        joystick_read_percent(joystick, &x, &y);
        bool pressed = joystick_is_pressed(joystick);

        // On n'affiche que si quelque chose a changé, pour garder la console lisible
        if (x != last_x || y != last_y || pressed != last_pressed) {
            ESP_LOGI(TAG, "X: %4d %%   Y: %4d %%   Z: %s", x, y, pressed ? "pressé" : "relâché");
            last_x = x;
            last_y = y;
            last_pressed = pressed;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void start_demo_14_joystick(void)
{
    static joystick_t joystick;
    joystick = joystick_init(JOYSTICK_GPIO_X, JOYSTICK_GPIO_Y, JOYSTICK_GPIO_Z);

    xTaskCreate(joystick_task, "14-joystick_task", 2048, &joystick, 1, NULL);
}
