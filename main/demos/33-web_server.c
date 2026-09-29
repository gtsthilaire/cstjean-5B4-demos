#include "web_server.h"
#include "wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_wifi.h"

static const char *TAG = "demos/33-web_server";

// Fournisseur de données : valeur aléatoire entre 20.00 et 29.99. Ça pourrait être une lecture de capteur réel.
// Appelée à chaque requête de la page (chaque seconde tant que la page est ouverte).
static float read_value(void)
{
    float value = 20.0f + (esp_random() % 1000) / 100.0f;
    ESP_LOGI(TAG, "Valeur envoyée : %.2f", value);
    return value;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        web_server_stop();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        web_server_start();
        ip_event_got_ip_t *e = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Serveur web ouvert : http://" IPSTR "/", IP2STR(&e->ip_info.ip));
    }
}

void start_demo_33_web_server(void)
{
    web_server_set_data_provider(read_value);

    // Voir la démo 32-wifi_station pour l'ordre des appels.
    wifi_setup();
    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    wifi_start_station(CONFIG_EXEMPLES_WIFI_SSID, CONFIG_EXEMPLES_WIFI_PASSWORD);
}
