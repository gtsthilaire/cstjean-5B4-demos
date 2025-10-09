/**
 * Démonstration de l'utilisation de MQTT.
 * On envoie et reçoit des données à Adafruit IO.
 * 
 * Le code est inspiré de l'exemple officiel "mqtt/tcp".
 * https://github.com/espressif/esp-idf/blob/v5.5.1/examples/protocols/mqtt/tcp/main/app_main.c
 */

#include "mqtt_aio.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_crt_bundle.h"

static const char *TAG = "mqtt_aio.c";

static esp_mqtt_client_handle_t s_client = NULL;
static const char* s_user = NULL;

static mqtt_connected_cb_t s_connected_callback = NULL;
static mqtt_msg_cb_t s_msg_callback = NULL;

static int  s_connected = 0;

static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            s_connected = 1;
            ESP_LOGI(TAG, "MQTT connecté");
    
            if (s_connected_callback) s_connected_callback(); // Appel de la fonction de callback pour notifier la connexion.
    
            break;
        case MQTT_EVENT_DISCONNECTED:
            s_connected = 0;
            ESP_LOGW(TAG, "MQTT déconnecté");
            break;
        case MQTT_EVENT_DATA:
            esp_mqtt_event_handle_t e = (esp_mqtt_event_handle_t)event_data;

            ESP_LOGI(TAG, "MSG %.*s: %.*s", e->topic_len, e->topic, e->data_len, e->data);

            if (s_msg_callback) s_msg_callback(e->topic, e->topic_len, e->data, e->data_len); // Appel de la fonction de callback pour notifier la réception d'un message.

            break;
        default: break;
    }
}

void mqtt_start(const char* user, const char* key)
{
    if (s_client) return; // Déjà démarré.
   
    s_user = user; // On stocke le nom d'utilisateur pour construire les topics lors des publications.

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtts://io.adafruit.com",
        .credentials.username = s_user,
        .credentials.authentication.password = key,
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
    };

    s_client = esp_mqtt_client_init(&mqtt_config);
    
    // On enregistre le gestionnaire d'événements pour gérer les événements MQTT.
    // C'est la fonction mqtt_event(...) ci-dessus qui sera appelée.
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event, NULL);
    
    esp_mqtt_client_start(s_client);
}

void mqtt_stop(void) 
{
    if (!s_client) return; // Pas démarré.

    esp_mqtt_client_stop(s_client);
    esp_mqtt_client_destroy(s_client);

    s_client = NULL;
    s_connected = 0;
    s_connected_callback = NULL;
    s_msg_callback = NULL;
}

void mqtt_publish_text(const char* topic, const char* payload, int retained) 
{
    if (!s_client || !s_connected || !topic[0]) return; // Pas démarré ou pas connecté ou topic vide.

    // On construit le topic complet pour Adafruit : "[user]/feeds/[topic]"
    char full_topic[128];
    snprintf(full_topic, sizeof(full_topic), "%s/feeds/%s", s_user, topic);

    esp_mqtt_client_publish(s_client, full_topic, payload, 0, 0, retained ? 1 : 0);
    ESP_LOGI(TAG, "PUB %s: %s", full_topic, payload);
}


void mqtt_set_connected_callback(mqtt_connected_cb_t callback) 
{
    s_connected_callback = callback;
}

void mqtt_set_message_callback(mqtt_msg_cb_t callback) 
{
    s_msg_callback = callback;
}

void mqtt_subscribe(const char* topic) 
{
    if (!s_client || !s_connected || !topic[0]) return; // Pas démarré ou pas connecté ou topic vide.

    // On construit le topic complet pour Adafruit : "[user]/feeds/[topic]"
    char full_topic[128];
    snprintf(full_topic, sizeof(full_topic), "%s/feeds/%s", s_user, topic);

    int mid = esp_mqtt_client_subscribe(s_client, full_topic, 0);
    ESP_LOGI(TAG, "SUB %s (id=%d)", full_topic, mid);
}