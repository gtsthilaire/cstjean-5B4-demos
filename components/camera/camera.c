/**
 * La caméra (OV2640 sur la carte Freenove ESP32-WROVER) est branchée en parallèle : 8 broches de données (D0 à D7)
 * plus des signaux de synchronisation (VSYNC, HREF, PCLK). Elle est configurée par un bus de type I2C (SCCB).
 * Elle a aussi besoin d'une horloge (XCLK) générée par l'ESP32 avec le périphérique LEDC.
 *
 * Les images sont trop grosses pour la RAM interne : elles sont stockées dans la PSRAM (mémoire externe du
 * module WROVER). La PSRAM doit donc être activée (CONFIG_SPIRAM, voir sdkconfig.defaults).
 *
 * On utilise le composant esp32-camera d'Espressif, déclaré dans idf_component.yml :
 * https://github.com/espressif/esp32-camera
 * 
 * Le code est inspiré de l'exemple officiel :
 * https://github.com/espressif/esp32-camera/blob/master/examples/camera_example/main/take_picture.c
 */

#include "camera.h"
#include "esp_log.h"

static const char *TAG = "components/camera";

// Brochage du Freenove ESP32-WROVER.
// Pour les autres cartes, voir https://github.com/espressif/esp32-camera/blob/master/examples/camera_example/main/camera_pinout.h
#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 21
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 19
#define CAM_PIN_D2 18
#define CAM_PIN_D1 5
#define CAM_PIN_D0 4
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

bool camera_init(void)
{
    ESP_LOGI(TAG, "Configuration de la caméra");

    camera_config_t camera_config = {
        .pin_pwdn = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        // Horloge XCLK de 20 MHz générée par le LEDC. On prend le timer 3 et le canal 7 pour ne pas entrer
        // en conflit avec les autres composants (led_pwm : timer 0, buzzer_passive : timer 1, servo : timer 2).
        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_3,
        .ledc_channel = LEDC_CHANNEL_7,

        .pixel_format = PIXFORMAT_JPEG,
        .frame_size = FRAMESIZE_QVGA, // 320x240

        .jpeg_quality = 12, // 0 à 63 : plus le nombre est petit, meilleure est la qualité
        .fb_count = 1,      // Nombre d'images gardées en mémoire
        .fb_location = CAMERA_FB_IN_PSRAM,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
    };

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_camera_init a échoué: %s", esp_err_to_name(err));
        return false;
    }

    return true;
}
