/**
 * Démonstration de l'utilisation d'un échange MQTT.
 * On envoie des données à Adafruit IO.
 * 
 * Le code est inspiré de l'exemple officiel "mqtt/tcp".
 * https://github.com/espressif/esp-idf/blob/v5.5.1/examples/protocols/mqtt/tcp/main/app_main.c
 */

#include "mqtt_aio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "esp_crt_bundle.h"

static const char *TAG = "mqtt.c";

static esp_mqtt_client_handle_t s_client = NULL;

static mqtt_msg_cb_t s_callback = NULL;

static char s_topic[128];
static int  s_connected = 0;

static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) 
{
    esp_mqtt_event_handle_t e = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            s_connected = 1;
            ESP_LOGI(TAG, "MQTT connecté");
    
            if (s_topic[0]) {
                int mid = esp_mqtt_client_subscribe(s_client, s_topic, 0);
                ESP_LOGI(TAG, "SUB %s (id=%d)", s_topic, mid);
            }
    
            break;
        case MQTT_EVENT_DISCONNECTED:
            s_connected = 0;
            ESP_LOGW(TAG, "MQTT déconnecté");
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MSG %.*s: %.*s", e->topic_len, e->topic, e->data_len, e->data);
            if (s_callback) s_callback(e->topic, e->topic_len, e->data, e->data_len);
            break;
        default: break;
    }
}

void mqtt_start(const char* user, const char* key, const char* feed)
{
    if (s_client) return;

    snprintf(s_topic, sizeof(s_topic), "%s/feeds/%s", user, feed ? feed : "");
    
    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtts://io.adafruit.com",
        .credentials.username = user,
        .credentials.authentication.password = key,
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
    };

    s_client = esp_mqtt_client_init(&mqtt_config);
    
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event, NULL);
    
    esp_mqtt_client_start(s_client);
}

void mqtt_stop(void) 
{
    if (!s_client) return;

    esp_mqtt_client_stop(s_client);
    esp_mqtt_client_destroy(s_client);

    s_client = NULL;
    s_connected = 0;
    s_callback = NULL;
}

void mqtt_publish_text(const char* payload, int retained) 
{
    if (!s_client || !s_connected || !s_topic[0]) return;

    esp_mqtt_client_publish(s_client, s_topic, payload, 0, 0, retained ? 1 : 0);
}

void mqtt_set_message_callback(mqtt_msg_cb_t callback) 
{
    s_callback = callback;
}