#include "mpu6050.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/26-mpu6050";

#define MPU6050_SDA_GPIO GPIO_NUM_13
#define MPU6050_SCL_GPIO GPIO_NUM_14
#define MPU6050_I2C_ADDR 0x68 // 0x69 si la broche AD0 du module est reliée au 3.3V

static void mpu6050_task(void *arg)
{
    mpu6050_t *mpu6050 = (mpu6050_t *)arg;

    while (1) {
        mpu6050_values_t values;
        mpu6050_read(mpu6050, &values);

        ESP_LOGI(TAG, "Accél (g): X=%5.2f Y=%5.2f Z=%5.2f   Rotation (°/s): X=%7.2f Y=%7.2f Z=%7.2f",
                 values.ax, values.ay, values.az, values.gx, values.gy, values.gz);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_26_mpu6050(void)
{
    static mpu6050_t mpu6050;
    mpu6050 = mpu6050_init(MPU6050_SDA_GPIO, MPU6050_SCL_GPIO, MPU6050_I2C_ADDR);

    xTaskCreate(mpu6050_task, "26-mpu6050_task", 3072, &mpu6050, 1, NULL);
}
