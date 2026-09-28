#include "wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"

static const char *TAG = "demos/32-wifi_ap";

#define WIFI_AP_CHANNEL 1  // Canal Wi-Fi (1 à 13)
#define WIFI_AP_MAX_CONN 4 // Nombre maximal d'appareils connectés en même temps

static void ap_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "WIFI: POINT D'ACCÈS DÉMARRÉ");
            break;
        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(TAG, "WIFI: POINT D'ACCÈS ARRÊTÉ");
            break;
        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "WIFI: UN APPAREIL S'EST CONNECTÉ");
            break;
        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "WIFI: UN APPAREIL S'EST DÉCONNECTÉ");
            break;
        default:
            break;
    }
}

void start_demo_32_wifi_ap(void)
{
    // Voir la démo 32-wifi_station pour l'ordre des appels.
    wifi_setup();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ap_event_handler, NULL, NULL);
    wifi_start_ap(CONFIG_EXEMPLES_AP_SSID, CONFIG_EXEMPLES_AP_PASSWORD, WIFI_AP_CHANNEL, WIFI_AP_MAX_CONN);
}
