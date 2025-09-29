/**
 * Démonstration de l'utilisation d'un SoftAP (Software Access Point).
 * Le SoftAP permet à d'autres appareils de se connecter à l'ESP32.
 *
 * ======================================================================================================
 * [!] Voir le component wifi_station pour une explication plus détaillée sur le fonctionnement du Wi-Fi.
 * ======================================================================================================
 * 
 * Le code est inspiré de l'exemple officiel "wifi/softAP".
 * https://github.com/espressif/esp-idf/blob/master/examples/wifi/getting_started/softAP/main/softap_example_main.c
 */

#include <string.h>
#include "wifi_ap.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "esp_mac.h"

static const char *TAG = "wifi_ap.c";

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi AP reçu: %ld", event_id);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            // WIFI_EVENT_AP_START : le point d'accès WiFi a démarré
            case WIFI_EVENT_AP_START:
                esp_netif_ip_info_t ip;
                esp_netif_t* ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
                if (ap_netif && esp_netif_get_ip_info(ap_netif, &ip) == ESP_OK) {
                    ESP_LOGI(TAG, "AP démarré. IP du SoftAP: " IPSTR, IP2STR(&ip.ip));
                } else {
                    ESP_LOGI(TAG, "AP démarré.");
                }
                break;

            // WIFI_EVENT_AP_STOP : le point d'accès WiFi a été arrêté
            case WIFI_EVENT_AP_STOP:
                ESP_LOGW(TAG, "AP arrêté");
                break;

            // WIFI_EVENT_AP_STACONNECTED : un client s'est connecté au point d'accès
            case WIFI_EVENT_AP_STACONNECTED: {
                const wifi_event_ap_staconnected_t* e = (const wifi_event_ap_staconnected_t*)event_data;
                ESP_LOGI(TAG, "STA connectée: " MACSTR ", AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }

            // WIFI_EVENT_AP_STADISCONNECTED : un client s'est déconnecté du point d'accès
            case WIFI_EVENT_AP_STADISCONNECTED: {
                const wifi_event_ap_stadisconnected_t* e = (const wifi_event_ap_stadisconnected_t*)event_data;
                ESP_LOGI(TAG, "STA déconnectée: " MACSTR ", AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }

            default:
                break;
        }
    }
}

void wifi_ap_start(const char* ssid, 
                    const char* password, 
                    int channel, 
                    int max_conn)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        NULL, NULL);


    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    
    wifi_config.ap.ssid_len     = strlen(ssid);
    strncpy((char*)wifi_config.ap.password, password ? password : "", sizeof(wifi_config.ap.password) - 1);
    
    wifi_config.ap.channel = (uint8_t)(channel <= 0 ? 1 : channel); // Canal Wi-Fi (1-13)
    wifi_config.ap.max_connection = (uint8_t)(max_conn <= 0 ? 4 : max_conn); // Nombre max de connexions
    wifi_config.ap.ssid_hidden = 0; // 0 = visible
    wifi_config.ap.authmode = (password && password[0]) ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN; // Mode de sécurité
    wifi_config.ap.beacon_interval = 100; // ms (par défaut)

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "SoftAP lancé: SSID=\"%s\" PASS=\"%s\" CH=%d MAX_CONN=%d",
             ssid, (password && password[0]) ? password : "<ouvert>", wifi_config.ap.channel, wifi_config.ap.max_connection);
}