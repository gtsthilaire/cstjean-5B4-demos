/**
 * Le MPU6050 est un capteur de mouvement qui contient deux capteurs :
 * - Un accéléromètre : il mesure l'accélération sur 3 axes (X, Y, Z). Au repos, il mesure quand même la gravité :
 *   à plat sur une table, on lit environ 1 g sur l'axe Z et 0 g sur X et Y. En regardant sur quel axe tombe la
 *   gravité, on peut savoir comment le capteur est incliné.
 * - Un gyroscope : il mesure la vitesse de rotation autour des 3 axes (en degrés par seconde). Au repos, il lit
 *   presque 0, mais jamais exactement : chaque capteur a un petit décalage.
 *
 * Il communique en I2C, avec seulement deux fils (SDA et SCL), comme l'écran LCD1602 (voir components/lcd1602).
 *
 * Le capteur range ses mesures dans des « registres » : de petites cases mémoire numérotées. Les 14 cases à partir
 * de la case 0x3B contiennent l'accélération X, Y, Z, la température, puis la rotation X, Y, Z. Chaque mesure
 * prend 2 cases (16 bits) : d'abord l'octet de poids fort, puis l'octet de poids faible. On les recolle avec
 * (fort << 8) | faible.
 *
 * Pour passer de la valeur brute à une unité lisible, on divise par un nombre donné par le fabricant
 * (réglages par défaut) :
 * - accélération : 16384 = 1 g (plage de -2 g à +2 g)
 * - rotation : 131 = 1 degré par seconde (plage de -250 à +250 °/s)
 *
 * À noter : chaque mpu6050_init crée son propre bus I2C (comme lcd1602_init). On ne peut donc pas (pour l'instant)
 * utiliser le MPU6050 et l'écran LCD1602 en même temps.
 */

#include "mpu6050.h"
#include "esp_log.h"

static const char *TAG = "components/mpu6050";

#define I2C_MASTER_NUM I2C_NUM_0  // Numéro du port I2C
#define I2C_MASTER_FREQ_HZ 100000 // Fréquence de l'horloge I2C
#define I2C_TIMEOUT_MS 100

// Numéros des registres utilisés (voir la documentation du MPU6050)
#define MPU6050_REG_PWR_MGMT_1 0x6B   // Gestion de l'alimentation (le capteur démarre en veille)
#define MPU6050_REG_WHO_AM_I 0x75     // Contient l'identifiant du capteur (0x68)
#define MPU6050_REG_ACCEL_XOUT_H 0x3B // Première des 14 cases qui contiennent les mesures

#define MPU6050_ACCEL_PER_G 16384.0f  // Valeur brute pour 1 g
#define MPU6050_GYRO_PER_DPS 131.0f   // Valeur brute pour 1 degré par seconde

mpu6050_t mpu6050_init(gpio_num_t sda, gpio_num_t scl, uint8_t i2c_addr)
{
    ESP_LOGI(TAG, "Configuration du bus I2C (SDA: GPIO %d, SCL: GPIO %d) et du capteur à l'adresse 0x%02X", sda, scl, i2c_addr);

    // Configuration du bus I2C. L'ESP32 est le master.
    i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7, // Valeur typique : filtre les parasites très courts sur les lignes
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    i2c_new_master_bus(&bus_config, &bus);

    // Ajout du capteur (le slave) sur le bus.
    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = i2c_addr,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    mpu6050_t mpu6050 = { .i2c_dev = NULL };
    i2c_master_bus_add_device(bus, &dev_config, &mpu6050.i2c_dev);

    // Le capteur démarre en veille : on écrit 0 dans le registre d'alimentation pour le réveiller.
    // Pour écrire dans un registre, on envoie 2 octets : le numéro du registre, puis la valeur.
    uint8_t wake_up[2] = { MPU6050_REG_PWR_MGMT_1, 0x00 };
    i2c_master_transmit(mpu6050.i2c_dev, wake_up, sizeof(wake_up), I2C_TIMEOUT_MS);

    // Lecture de l'identifiant : 0x68 si le capteur est bien branché. Pratique pour vérifier le câblage.
    // Certains modules contiennent un MPU6500 compatible, qui répond 0x70 : il fonctionne quand même.
    // Pour lire un registre, on envoie son numéro, puis on lit la réponse.
    uint8_t reg = MPU6050_REG_WHO_AM_I;
    uint8_t who_am_i = 0;
    i2c_master_transmit_receive(mpu6050.i2c_dev, &reg, 1, &who_am_i, 1, I2C_TIMEOUT_MS);
    ESP_LOGI(TAG, "Identifiant du capteur : 0x%02X (0x68 = MPU6050, 0x70 = MPU6500 compatible)", who_am_i);

    return mpu6050;
}

void mpu6050_read_raw(mpu6050_t *mpu6050, mpu6050_raw_t *raw)
{
    if (mpu6050 == NULL || raw == NULL) {
        return;
    }

    // On lit les 14 cases d'un coup, à partir de la première mesure.
    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;
    uint8_t buf[14] = { 0 };
    i2c_master_transmit_receive(mpu6050->i2c_dev, &reg, 1, buf, sizeof(buf), I2C_TIMEOUT_MS);

    // Chaque mesure prend 2 octets : on recolle l'octet de poids fort et l'octet de poids faible.
    // Les cases 6 et 7 (température de la puce) ne sont pas utilisées.
    raw->ax = (int16_t)((buf[0] << 8) | buf[1]);
    raw->ay = (int16_t)((buf[2] << 8) | buf[3]);
    raw->az = (int16_t)((buf[4] << 8) | buf[5]);
    raw->gx = (int16_t)((buf[8] << 8) | buf[9]);
    raw->gy = (int16_t)((buf[10] << 8) | buf[11]);
    raw->gz = (int16_t)((buf[12] << 8) | buf[13]);
}

void mpu6050_read(mpu6050_t *mpu6050, mpu6050_values_t *values)
{
    if (mpu6050 == NULL || values == NULL) {
        return;
    }

    mpu6050_raw_t raw;
    mpu6050_read_raw(mpu6050, &raw);

    values->ax = raw.ax / MPU6050_ACCEL_PER_G;
    values->ay = raw.ay / MPU6050_ACCEL_PER_G;
    values->az = raw.az / MPU6050_ACCEL_PER_G;
    values->gx = raw.gx / MPU6050_GYRO_PER_DPS;
    values->gy = raw.gy / MPU6050_GYRO_PER_DPS;
    values->gz = raw.gz / MPU6050_GYRO_PER_DPS;
}
