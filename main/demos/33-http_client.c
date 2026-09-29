#include "thingspeak.h"
#include "wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_wifi.h"

static const char *TAG = "demos/33-http_client";

// ThingSpeak n'accepte qu'une mise à jour toutes les 15 secondes au minimum.
#define HTTP_CLIENT_PERIOD_MS 20000

// Fournisseur de données : valeur aléatoire entre 20.00 et 29.99. Ça pourrait être une lecture de capteur réel.
static float read_value(void)
{
    float value = 20.0f + (esp_random() % 1000) / 100.0f;
    ESP_LOGI(TAG, "Valeur : %.2f", value);
    return value;
}

// Dès que l'ESP32 obtient une adresse IP, on démarre l'envoi. Aux reconnexions suivantes, thingspeak_start ne fait rien.
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    thingspeak_start(CONFIG_EXEMPLES_TS_API_KEY, HTTP_CLIENT_PERIOD_MS);
}

void start_demo_33_http_client(void)
{
    // Même fournisseur pour les deux champs, pour l'exemple
    thingspeak_set_data_provider(read_value, read_value);

    // Voir la démo 32-wifi_station pour l'ordre des appels.
    wifi_setup();
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    wifi_start_station(CONFIG_EXEMPLES_WIFI_SSID, CONFIG_EXEMPLES_WIFI_PASSWORD);
}
