#include "wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"

static const char *TAG = "demos/32-wifi_station";

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "WIFI: CONNEXION EN COURS...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI: DÉCONNECTÉ");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *e = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "WIFI: CONNECTÉ (IP: " IPSTR ")", IP2STR(&e->ip_info.ip));
    }
}

void start_demo_32_wifi_station(void)
{
    // Les gestionnaires doivent être enregistrés après wifi_setup() (qui crée la boucle d'événements)
    // et avant wifi_start_station() (pour ne pas manquer les premiers événements).
    wifi_setup();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    wifi_start_station(CONFIG_EXEMPLES_WIFI_SSID, CONFIG_EXEMPLES_WIFI_PASSWORD);
}
