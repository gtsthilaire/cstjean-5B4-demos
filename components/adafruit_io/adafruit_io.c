/**
 * Client MQTT pour Adafruit IO (https://io.adafruit.com).
 *
 * MQTT est un protocole de messagerie léger, très utilisé en IoT. Les appareils se connectent à un serveur
 * central (le broker) et échangent des messages classés par sujet (topic) :
 * - publier (publish) : envoyer un message sur un topic
 * - s'abonner (subscribe) : recevoir tous les messages publiés sur un topic
 *
 * Chez Adafruit IO, les topics s'appellent des feeds et ont la forme « [user]/feeds/[feed] ».
 * Ce composant construit le topic complet : l'application n'a qu'à donner le nom du feed.
 *
 * La connexion est chiffrée (mqtts://, port 8883) : le certificat du broker est vérifié avec le paquet de
 * certificats racines inclus dans ESP-IDF (esp_crt_bundle_attach).
 *
 * La connexion n'est jamais arrêtée : le client MQTT d'ESP-IDF se reconnecte tout seul après une coupure
 * (Wi-Fi ou broker). Pendant la coupure, les publications sont simplement ignorées.
 *
 * Comme il n'y a qu'une seule connexion, il n'y a pas de adafruit_io_t : l'état est gardé dans des variables static.
 *
 * Le code est inspiré de l'exemple officiel « mqtt/tcp ».
 * https://github.com/espressif/esp-mqtt/tree/master/examples/tcp
 */

#include "adafruit_io.h"
#include <stdbool.h>
#include <stdio.h>
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "components/adafruit_io";

static esp_mqtt_client_handle_t s_client = NULL;
static const char *s_user = NULL;

static adafruit_io_connected_cb_t s_connected_callback = NULL;
static adafruit_io_message_cb_t s_message_callback = NULL;

static bool s_connected = false;

// Gestionnaire des événements MQTT (connexion, déconnexion, message reçu).
static void mqtt_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            s_connected = true;
            ESP_LOGI(TAG, "MQTT connecté");
            if (s_connected_callback != NULL) {
                s_connected_callback(); // On prévient l'application
            }
            break;

        case MQTT_EVENT_DISCONNECTED:
            s_connected = false;
            ESP_LOGW(TAG, "MQTT déconnecté (reconnexion automatique)");
            break;

        case MQTT_EVENT_DATA: {
            esp_mqtt_event_handle_t e = (esp_mqtt_event_handle_t)event_data;
            if (s_message_callback != NULL) {
                s_message_callback(e->topic, e->topic_len, e->data, e->data_len); // On transmet le message à l'application
            }
            break;
        }

        default:
            break;
    }
}

void adafruit_io_set_connected_callback(adafruit_io_connected_cb_t callback)
{
    s_connected_callback = callback;
}

void adafruit_io_set_message_callback(adafruit_io_message_cb_t callback)
{
    s_message_callback = callback;
}

void adafruit_io_start(const char *user, const char *key)
{
    if (s_client != NULL) {
        return; // Déjà démarré
    }

    s_user = user; // On garde le nom d'utilisateur pour construire les topics

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtts://io.adafruit.com",
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
        .credentials.username = user,
        .credentials.authentication.password = key,
    };

    s_client = esp_mqtt_client_init(&mqtt_config);

    // On enregistre le gestionnaire d'événements : c'est la fonction mqtt_event(...) ci-dessus qui sera appelée.
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event, NULL);

    esp_mqtt_client_start(s_client);
}

void adafruit_io_publish_text(const char *feed, const char *payload)
{
    if (s_client == NULL || !s_connected) {
        return; // Pas encore connecté (ou connexion coupée) : on ne publie rien
    }

    // On construit le topic complet pour Adafruit IO : "[user]/feeds/[feed]"
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/feeds/%s", s_user, feed);

    esp_mqtt_client_publish(s_client, topic, payload, 0, 0, 0);
    ESP_LOGI(TAG, "Publié sur %s : %s", topic, payload);
}

void adafruit_io_publish_float(const char *feed, float value)
{
    // MQTT n'envoie que du texte : on convertit le nombre en chaîne de caractères avec deux décimales.
    char payload[32];
    snprintf(payload, sizeof(payload), "%.2f", value);
    adafruit_io_publish_text(feed, payload);
}

void adafruit_io_subscribe(const char *feed)
{
    if (s_client == NULL || !s_connected) {
        return; // Pas encore connecté : on s'abonnera dans le callback de connexion
    }

    // On construit le topic complet pour Adafruit IO : "[user]/feeds/[feed]"
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/feeds/%s", s_user, feed);

    esp_mqtt_client_subscribe(s_client, topic, 0);
    ESP_LOGI(TAG, "Abonné à %s", topic);
}
