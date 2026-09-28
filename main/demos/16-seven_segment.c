#include "seven_segment.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/16-seven_segment";

#define SEVEN_SEGMENT_GPIO_DATA GPIO_NUM_15
#define SEVEN_SEGMENT_GPIO_CLOCK GPIO_NUM_4
#define SEVEN_SEGMENT_GPIO_LATCH GPIO_NUM_2

#define SEVEN_SEGMENT_DELAY_MS 1000

static void seven_segment_task(void *arg)
{
    seven_segment_t *seven_segment = (seven_segment_t *)arg;

    while (1) {
        for (int digit = 0; digit < 16; digit++) {
            ESP_LOGI(TAG, "Chiffre : %X", digit); // %X affiche en hexadécimal : 0 à 9, puis A à F
            seven_segment_show_digit(seven_segment, digit);
            vTaskDelay(pdMS_TO_TICKS(SEVEN_SEGMENT_DELAY_MS));
        }
    }
}

void start_demo_16_seven_segment(void)
{
    static seven_segment_t seven_segment;
    seven_segment = seven_segment_init(SEVEN_SEGMENT_GPIO_DATA, SEVEN_SEGMENT_GPIO_CLOCK, SEVEN_SEGMENT_GPIO_LATCH);

    xTaskCreate(seven_segment_task, "16-seven_segment_task", 2048, &seven_segment, 1, NULL);
}
