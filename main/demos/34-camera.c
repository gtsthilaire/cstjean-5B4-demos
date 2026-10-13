#include "camera.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/34-camera";

static void camera_task(void *arg)
{
    while (1) {
        ESP_LOGI(TAG, "Prise de photo...");
        camera_fb_t *pic = esp_camera_fb_get();

        if (pic == NULL) {
            ESP_LOGE(TAG, "Échec de la capture");
        } else {
            ESP_LOGI(TAG, "Format = %s", (pic->format == PIXFORMAT_JPEG) ? "JPEG" : "RAW");
            ESP_LOGI(TAG, "Taille = %d octets", (int)pic->len);
            ESP_LOGI(TAG, "Dimensions = %dx%d", (int)pic->width, (int)pic->height);

            // Ici, on affiche seulement des informations sur l'image, sans l'utiliser.
            // L'image elle-même est dans pic->buf : c'est un fichier JPEG complet de pic->len octets.
            // On pourrait par exemple :
            //   - l'envoyer à un navigateur pour l'afficher dans une page web (voir la démo 33-web_server) ;
            //   - l'envoyer par Wi-Fi à un serveur (voir la démo 33-http_client).
            // Attention : il faut s'en servir avant esp_camera_fb_return(), qui rend la mémoire à la caméra.

            esp_camera_fb_return(pic); // Libérer la mémoire de l'image
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void start_demo_34_camera(void)
{
    camera_init();

    xTaskCreate(camera_task, "34-camera_task", 4096, NULL, 1, NULL);
}
