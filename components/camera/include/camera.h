#ifndef CAMERA_H
#define CAMERA_H

#include "esp_camera.h"

// Configure la caméra (brochage du Freenove ESP32-WROVER, JPEG 320x240).
// Ensuite, on prend une photo avec esp_camera_fb_get() et on la libère avec esp_camera_fb_return().
void camera_init(void);

#endif // CAMERA_H
