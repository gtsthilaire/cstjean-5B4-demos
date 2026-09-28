#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include "driver/gpio.h"
#include "driver/i2c_master.h"

typedef struct {
    i2c_master_dev_handle_t i2c_dev; // périphérique I2C (le capteur MPU6050)
} mpu6050_t;

// Valeurs brutes lues dans le capteur (nombres entiers de -32768 à 32767).
typedef struct {
    int16_t ax, ay, az; // accélération sur les axes X, Y et Z
    int16_t gx, gy, gz; // vitesse de rotation autour des axes X, Y et Z
} mpu6050_raw_t;

// Valeurs converties en unités lisibles.
typedef struct {
    float ax, ay, az; // accélération (en g : 1 g = la gravité terrestre)
    float gx, gy, gz; // vitesse de rotation (en degrés par seconde)
} mpu6050_values_t;

// Crée le bus I2C, ajoute le capteur à l'adresse donnée (0x68 par défaut), le réveille et retourne un mpu6050_t.
mpu6050_t mpu6050_init(gpio_num_t sda, gpio_num_t scl, uint8_t i2c_addr);

// Lit les valeurs brutes de l'accéléromètre et du gyroscope.
void mpu6050_read_raw(mpu6050_t *mpu6050, mpu6050_raw_t *raw);

// Lit l'accélération (en g) et la vitesse de rotation (en degrés par seconde).
void mpu6050_read(mpu6050_t *mpu6050, mpu6050_values_t *values);

#endif // MPU6050_H
