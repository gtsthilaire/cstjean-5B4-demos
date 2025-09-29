/**
 * Démonstration de l'utilisation d'une connexion Wi-Fi.
 * 
 * Le code est inspiré de l'exemple officiel "wifi/station".
 * https://github.com/espressif/esp-idf/blob/master/examples/wifi/getting_started/station/main/station_example_main.c
 */

#include <string.h>
#include "wifi_station.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_err.h"

static const char *TAG = "wifi_station.c";

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu: %ld", event_id);

    // WIFI_EVENT_STA_START : la station WiFi a démarré
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Station WiFi démarrée, tentative de connexion...");
        esp_wifi_connect();
    }

    // WIFI_EVENT_STA_DISCONNECTED : la station WiFi a été déconnectée
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        const wifi_event_sta_disconnected_t* d = (const wifi_event_sta_disconnected_t*)event_data;
        ESP_LOGI(TAG, "Déconnecté (raison=%d), nouvelle tentative...", d->reason);
        esp_wifi_connect();
    }

    // IP_EVENT_STA_GOT_IP : la station WiFi a obtenu une adresse IP
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "IP obtenue: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void wifi_start(const char* ssid,
                const char* password)
{
    // Attention, certaines de ces étapes doivent être effectuées qu'une seule fois,
    // généralement au démarrage de l'application. Pour faire simple, elles sont incluses ici.
    // Dans une application réelle, vous devriez vérifier si elles ont déjà été faites.

    // Initialisation de la mémoire NVS (Non-Volatile Storage)
    // Obligatoire pour le WiFi, car il utilise cette mémoire pour stocker
    // des paramètres comme le SSID et le mot de passe.
    esp_err_t err = nvs_flash_init();

    // Si la mémoire NVS est pleine ou si une nouvelle version est détectée,
    // on efface la mémoire et on réinitialise.
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Initialisation de la pile TCP/IP.
    // Nécessaire avant de créer des interfaces réseau.
    esp_netif_init();

    // Création de la boucle d'événements par défaut.
    // Nécessaire pour gérer les événements système, y compris ceux du WiFi.
    esp_event_loop_create_default();

    // Création de l'interface réseau WiFi par défaut en mode station.
    // Cela configure l'ESP32 pour se connecter à un point d'accès WiFi.
    esp_netif_create_default_wifi_sta();

    // Configuration par défaut du WiFi.
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    // Enregistrement du gestionnaire d'événements pour les événements WiFi.
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &event_handler,
                                        NULL, NULL);

    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &event_handler,
                                        NULL, NULL);
                                        
    // Configuration du SSID et du mot de passe pour la connexion WiFi.
    // On utilise strncpy pour éviter les débordements de mémoire.    
    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // Configuration du mode WiFi en mode station (STA).
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    // Démarrage du WiFi avec la configuration spécifiée.
    esp_wifi_start();

    ESP_LOGI(TAG, "Connexion Wi-Fi lancée (SSID: %s)", ssid);
}