#include "adafruit_io.h"
#include "wifi.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_random.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "demos/33-mqtt";

static const char *FEED = "demo-5b4"; // Le feed « demo-5b4 » doit être créé au préalable sur Adafruit IO.

#define MQTT_PUBLISH_PERIOD_MS 5000

// --- CONNEXION au broker : nécessaire pour PUBLISH et pour SUBSCRIBE ---

// Dès que l'ESP32 obtient une adresse IP, on se connecte au broker. Aux reconnexions suivantes du Wi-Fi,
// adafruit_io_start ne fait rien : le client MQTT se reconnecte tout seul.
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    adafruit_io_start(CONFIG_EXEMPLES_AIO_USER, CONFIG_EXEMPLES_AIO_KEY);
}

// --- SUBSCRIBE : recevoir les messages du feed ---

// Appelée à chaque message reçu sur un feed auquel on est abonné.
// Le topic et le payload ne se terminent pas par '\0' : on utilise %.*s avec leur longueur.
static void on_message(const char *topic, size_t topic_len, const char *payload, size_t payload_len)
{
    ESP_LOGI(TAG, "Reçu [%.*s] => %.*s", (int)topic_len, topic, (int)payload_len, payload);
}

// Appelée à chaque connexion (et reconnexion) au broker : on doit se réabonner à chaque fois.
static void on_connected(void)
{
    adafruit_io_subscribe(FEED);
}

// --- PUBLISH : envoyer une valeur sur le feed ---

// Publie périodiquement une valeur. Tant que la connexion n'est pas établie, adafruit_io_publish_float ne fait rien.
static void mqtt_publish_task(void *arg)
{
    while (1) {
        float value = 20.0f + (esp_random() % 1000) / 100.0f; // Ça pourrait être une lecture de capteur réel.
        adafruit_io_publish_float(FEED, value);

        vTaskDelay(pdMS_TO_TICKS(MQTT_PUBLISH_PERIOD_MS));
    }
}

void start_demo_33_mqtt(void)
{
    // SUBSCRIBE (à retirer pour seulement publier)
    adafruit_io_set_connected_callback(on_connected);
    adafruit_io_set_message_callback(on_message);

    // CONNEXION (à garder dans tous les cas) : Wi-Fi, puis broker dès qu'on a une adresse IP.
    // Voir la démo 32-wifi_station pour l'ordre des appels.
    wifi_setup();
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    wifi_start_station(CONFIG_EXEMPLES_WIFI_SSID, CONFIG_EXEMPLES_WIFI_PASSWORD);

    // PUBLISH (à retirer pour seulement s'abonner)
    xTaskCreate(mqtt_publish_task, "33-mqtt_publish_task", 4096, NULL, 1, NULL);
}
