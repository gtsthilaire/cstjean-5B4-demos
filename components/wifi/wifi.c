/**
 * Gestion du Wi-Fi de l'ESP32, en mode station (on se connecte à un réseau existant) et/ou en mode point d'accès
 * (SoftAP : l'ESP32 crée son propre réseau et d'autres appareils s'y connectent).
 *
 * Le Wi-Fi fonctionne avec des événements : on lance une action (ex : esp_wifi_connect) et on est prévenu plus tard,
 * par la boucle d'événements, du résultat (ex : WIFI_EVENT_STA_DISCONNECTED ou IP_EVENT_STA_GOT_IP).
 * Ce composant gère lui-même les événements nécessaires (connexion, reconnexion, logs). L'application peut
 * enregistrer ses propres gestionnaires pour réagir aux mêmes événements (voir les démos 32 et 33).
 *
 * Contrairement aux autres composants, il n'y a pas de wifi_t : l'ESP32 n'a qu'une seule radio Wi-Fi, la pile
 * TCP/IP et la boucle d'événements ne doivent être initialisées qu'une seule fois. wifi_setup() s'en assure, ce qui
 * permet aussi d'utiliser les deux modes en même temps (station + point d'accès).
 *
 * Le code est inspiré des exemples officiels « wifi/getting_started/station » et « wifi/getting_started/softAP ».
 * https://github.com/espressif/esp-idf/tree/master/examples/wifi/getting_started
 */

#include "wifi.h"
#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "components/wifi";

static bool s_initialized = false;
static wifi_mode_t s_mode = WIFI_MODE_NULL; // mode actuel : station, point d'accès ou les deux

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            // La station Wi-Fi a démarré : on lance la connexion
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "Station démarrée, tentative de connexion...");
                esp_wifi_connect();
                break;

            // La station a été déconnectée (ou la connexion a échoué) : on réessaie
            case WIFI_EVENT_STA_DISCONNECTED: {
                const wifi_event_sta_disconnected_t *e = (const wifi_event_sta_disconnected_t *)event_data;

                // Les deux erreurs les plus courantes : mauvais SSID ou mauvais mot de passe
                if (e->reason == WIFI_REASON_NO_AP_FOUND) {
                    ESP_LOGW(TAG, "Réseau introuvable : vérifier le SSID");
                } else if (e->reason == WIFI_REASON_AUTH_FAIL || e->reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT ||
                           e->reason == WIFI_REASON_HANDSHAKE_TIMEOUT) {
                    ESP_LOGW(TAG, "Mot de passe probablement incorrect");
                }

                ESP_LOGW(TAG, "Déconnecté (raison=%d), nouvelle tentative...", e->reason);
                esp_wifi_connect();
                break;
            }

            // Le point d'accès a démarré
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "Point d'accès démarré");
                break;

            // Un appareil s'est connecté au point d'accès
            case WIFI_EVENT_AP_STACONNECTED: {
                const wifi_event_ap_staconnected_t *e = (const wifi_event_ap_staconnected_t *)event_data;
                ESP_LOGI(TAG, "Appareil connecté: " MACSTR ", AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }

            // Un appareil s'est déconnecté du point d'accès
            case WIFI_EVENT_AP_STADISCONNECTED: {
                const wifi_event_ap_stadisconnected_t *e = (const wifi_event_ap_stadisconnected_t *)event_data;
                ESP_LOGI(TAG, "Appareil déconnecté: " MACSTR ", AID=%d", MAC2STR(e->mac), e->aid);
                break;
            }

            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // La station a obtenu une adresse IP (via DHCP) : la connexion est complète
        const ip_event_got_ip_t *e = (const ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "IP obtenue: " IPSTR, IP2STR(&e->ip_info.ip));
    }
}

void wifi_setup(void)
{
    if (s_initialized) {
        return;
    }

    ESP_LOGI(TAG, "Initialisation du Wi-Fi");

    // Initialisation de la mémoire NVS (Non-Volatile Storage).
    // Obligatoire pour le Wi-Fi, car il utilise cette mémoire pour stocker des paramètres (calibration radio, etc.).
    // Si la mémoire NVS est pleine ou si une nouvelle version est détectée, on l'efface et on réinitialise.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Initialisation de la pile TCP/IP. Nécessaire avant de créer des interfaces réseau.
    esp_netif_init();

    // Création de la boucle d'événements par défaut. Nécessaire pour recevoir les événements système (dont ceux du Wi-Fi).
    esp_event_loop_create_default();

    // Création des interfaces réseau par défaut : une pour la station, une pour le point d'accès.
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    // Initialisation du pilote Wi-Fi avec la configuration par défaut.
    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    // Gestionnaire d'événements interne (connexion, reconnexion, logs).
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL);

    s_initialized = true;
}

// Ajoute un mode (station ou point d'accès) au mode actuel. Retourne true si le Wi-Fi était déjà démarré.
static bool add_mode(wifi_mode_t mode)
{
    bool already_started = s_mode != WIFI_MODE_NULL;

    s_mode = (s_mode == WIFI_MODE_NULL || s_mode == mode) ? mode : WIFI_MODE_APSTA;
    esp_wifi_set_mode(s_mode);

    return already_started;
}

void wifi_start_station(const char *ssid, const char *password)
{
    if (!s_initialized || ssid == NULL) {
        return;
    }

    // Configuration du SSID et du mot de passe. On utilise strncpy pour éviter les débordements de mémoire.
    bool has_password = password != NULL && password[0] != '\0';

    wifi_config_t wifi_config = { 0 };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, has_password ? password : "", sizeof(wifi_config.sta.password) - 1);
    // Sécurité minimale acceptée : WPA2 si on a un mot de passe, sinon réseau ouvert
    wifi_config.sta.threshold.authmode = has_password ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;

    bool already_started = add_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

    if (!already_started) {
        esp_wifi_start(); // La connexion sera lancée à la réception de WIFI_EVENT_STA_START (voir event_handler)
    } else {
        esp_wifi_connect(); // Le Wi-Fi tournait déjà (en point d'accès) : on se connecte directement
    }

    ESP_LOGI(TAG, "Connexion Wi-Fi lancée (SSID: %s)", ssid);
}

void wifi_start_ap(const char *ssid, const char *password, int channel, int max_conn)
{
    if (!s_initialized || ssid == NULL) {
        return;
    }

    bool has_password = password != NULL && password[0] != '\0';

    wifi_config_t wifi_config = { 0 };
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    wifi_config.ap.ssid_len = strlen((char *)wifi_config.ap.ssid);
    strncpy((char *)wifi_config.ap.password, has_password ? password : "", sizeof(wifi_config.ap.password) - 1);
    wifi_config.ap.channel = (uint8_t)(channel <= 0 ? 1 : channel);              // Canal Wi-Fi (1-13)
    wifi_config.ap.max_connection = (uint8_t)(max_conn <= 0 ? 4 : max_conn);     // Nombre max de connexions
    wifi_config.ap.authmode = has_password ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN; // Mode de sécurité
    wifi_config.ap.beacon_interval = 100;                                         // ms (par défaut)

    bool already_started = add_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);

    if (!already_started) {
        esp_wifi_start();
    }

    // L'adresse IP du point d'accès (192.168.4.1 par défaut) : c'est l'adresse à utiliser depuis un appareil connecté.
    esp_netif_ip_info_t ip;
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (ap_netif != NULL && esp_netif_get_ip_info(ap_netif, &ip) == ESP_OK) {
        ESP_LOGI(TAG, "Point d'accès lancé: SSID=\"%s\" CH=%d MAX_CONN=%d IP=" IPSTR, ssid, wifi_config.ap.channel,
                 wifi_config.ap.max_connection, IP2STR(&ip.ip));
    }
}
