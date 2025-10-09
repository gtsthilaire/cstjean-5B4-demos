#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_random.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_config.h"
#include "led.h"
#include "push_button.h"
#include "led_bar.h"
#include "led_pwm.h"
#include "led_rgb.h"
#include "led_pixel.h"
#include "buzzer_active.h"
#include "buzzer_passive.h"
#include "potentiometer.h"
#include "touch.h"
#include "photoresistor.h"
#include "thermistor.h"
#include "lcd1602.h"
#include "servo.h"
#include "joystick.h"
#include "wifi_ap.h"
#include "wifi_station.h"
#include "web_server.h"
#include "http_client_ts.h"
#include "mqtt_aio.h"
#include "led_matrix.h"

static const char* TAG = "exemples";

// ===========================
// Exemples pour la station Wi-Fi
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "WIFI: CONNECTING ...");
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGI(TAG, "WIFI: DISCONNECTED");
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WIFI: CONNECTED");
    }
}

// ===========================
// Exemples pour le point d'accès Wi-Fi
static void ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement AP reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "WIFI: AP START");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGI(TAG, "WIFI: AP STOP");
                break;
            case WIFI_EVENT_AP_STACONNECTED: {
                ESP_LOGI(TAG, "WIFI: STA CONNECTED");
                break;
            }
            case WIFI_EVENT_AP_STADISCONNECTED: {
                ESP_LOGI(TAG, "WIFI: STA DISCONNECTED");
                break;
            }
            default:
                break;
        }
    }
}

// ===========================
// Exemples pour le serveur web
static bool data_provider(float* out_data) 
{
    if (!out_data) return false;
    // Valeur aléatoire entre 20.00 et 29.99 pour l'exemple. Ça pourrait être une lecture de capteur réel.
    float t = 20.f + (esp_random() % 1000) / 100.f;
    ESP_LOGI(TAG, "Donnée: %.2f", t);
    *out_data = t;
    return true;
}

static void webserver_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI: DISCONNECTED");
        web_server_stop();
        ESP_LOGI(TAG, "Serveur web arrêté");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WIFI: CONNECTED");
        web_server_set_data_provider(data_provider);
        web_server_start();
        ip_event_got_ip_t* e = (ip_event_got_ip_t*) event_data;   
        ESP_LOGI(TAG, "Serveur web ouvert : http://" IPSTR "/", IP2STR(&e->ip_info.ip));
    }
}

// ===========================
// Exemples pour le client HTTP
static void http_client_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI: DISCONNECTED");
        stop_demo_http_client_task();
        ESP_LOGI(TAG, "Client HTTP arrêté");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WIFI: CONNECTED");
        http_client_set_data_provider(data_provider, data_provider);
        start_demo_http_client_task(TS_API_KEY, 20000); // Toutes les 20 secondes car Thingspeak n'accepte qu'une mise à jour toutes les 15 secondes au minimum.
        ESP_LOGI(TAG, "Client HTTP démarré");
    }
}

// ===========================
// Exemples pour MQTT
// Si vous voulez utiliser les deux (subscribe et publish), vous pouvez fusionner les deux event handler.

static const char* s_topic = "demo"; // Le feed "demo" doit être créé au préalable sur Adafruit IO.

static void mqtt_on_msg(const char* topic, size_t topic_len, const char* payload, size_t payload_len)
{
    ESP_LOGI(TAG, "Reçu MQTT [%.*s] => %.*s", topic_len, topic, payload_len, payload);
}

static void mqtt_subscribe_on_connected(void)
{
    mqtt_subscribe(s_topic);
}

static void mqtt_subscribe_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI: DISCONNECTED");
        mqtt_stop();
        ESP_LOGI(TAG, "Client MQTT arrêté");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WIFI: CONNECTED");
        mqtt_set_connected_callback(mqtt_subscribe_on_connected);
        mqtt_set_message_callback(mqtt_on_msg);
        mqtt_start(AIO_USER, AIO_KEY);
        ESP_LOGI(TAG, "Client MQTT démarré");
    }
}

// Une tâche qui publie périodiquement des données via MQTT.
static void mqtt_pub_task(void* arg) 
{
    while(1) {
        float value = 0.f;
        if (data_provider(&value)) {
            mqtt_publish_float(s_topic, value, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void mqtt_publish_on_connected(void)
{
    xTaskCreate(mqtt_pub_task, "mqtt_pub_task", 4096, NULL, 5, NULL); // Ici, on simule un autre component qui publie périodiquement des données via MQTT.
}

static void mqtt_publish_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    ESP_LOGI(TAG, "Événement Wi-Fi reçu (main): %ld", event_id);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "WIFI: DISCONNECTED");
        mqtt_stop();
        ESP_LOGI(TAG, "Client MQTT arrêté");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WIFI: CONNECTED");
        mqtt_set_connected_callback(mqtt_publish_on_connected);
        mqtt_start(AIO_USER, AIO_KEY);
        ESP_LOGI(TAG, "Client MQTT démarré");
    }
}

void app_main(void)
{
    // components/led/led.c
    // start_demo_led_task(GPIO_NUM_2);

    // components/push_button/push_button.c
    // start_demo_push_button_task(GPIO_NUM_2);

    // components/led_bar/led_bar.c
    // gpio_num_t led_pins[] =  {GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_0, GPIO_NUM_2, GPIO_NUM_15};
    // start_demo_led_bar_task(led_pins, sizeof(led_pins) / sizeof(led_pins[0]));

    // components/led_pwm/led_pwm.c
    // start_demo_led_pwm_task(GPIO_NUM_2);

    // components/led_rgb/led_rgb.c
    // start_demo_led_rgb_task(GPIO_NUM_15, GPIO_NUM_2, GPIO_NUM_4);

    // components/led_pixel/led_pixel.c
    // start_demo_led_pixel_task(GPIO_NUM_2);

    // components/buzzer_active/buzzer_active.c
    // start_demo_buzzer_active_task(GPIO_NUM_13);

    // components/buzzer_passive/buzzer_passive.c
    // start_demo_buzzer_passive_task(GPIO_NUM_13);

    // components/potentiometer/potentiometer.c
    // start_demo_potentiometer_task(ADC_CHANNEL_6); // GPIO34

    // components/touch/touch.c
    // start_demo_touch_task(TOUCH_PAD_NUM0); // GPIO4

    // components/photoresistor/photoresistor.c
    // start_demo_photoresistor_task(ADC_CHANNEL_6); // GPIO34

    // components/thermistor/thermistor.c
    // start_demo_thermistor_task(ADC_CHANNEL_6); // GPIO34

    // components/lcd1602/lcd1602.c
    // start_demo_lcd1602_task();

    // components/servo/servo.c
    // start_demo_servo_task(GPIO_NUM_15);

    // * ======================================================================================================
    // * [!] Les handlers doivent être enregistrés après l'initialisation de la pile TCP/IP
    // * et la création de la boucle d'événements. On doit donc découper l'initialisation WiFi en deux fonctions:
    // * - wifi_init() : Initialise la pile TCP/IP, la mémoire NVS, la boucle d'événements et l'interface réseau WiFi.
    // * - wifi_start() : Configure le SSID et le mot de passe, enregistre les handlers et démarre le WiFi.
    // * ======================================================================================================

    // components/wifi_station/wifi_station.c
    // wifi_init();
    // esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    // esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);
    // wifi_start(WIFI_SSID, WIFI_PASSWORD);
    
    // * ======================================================================================================
    // * [!] Comme pour le WiFi en mode station, les handlers doivent être enregistrés après l'initialisation
    // * de la pile TCP/IP et la création de la boucle d'événements. La mécanique actuelle ne nous permet pas
    // * d'avoir les deux modes (station et AP) en même temps. Il faudrait modifier la logique pour permettre
    // * d'initialiser la pile TCP/IP et la boucle d'événements qu'une seule fois.
    // * ======================================================================================================

    // components/wifi_ap/wifi_ap.c
    // wifi_ap_init();
    // esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ap_event_handler, NULL, NULL);
    // wifi_ap_start(WIFI_SSID, WIFI_PASSWORD, 1, 4);

    // components/joystick/joystick.c
    // start_demo_joystick_task(ADC_CHANNEL_4, ADC_CHANNEL_5, GPIO_NUM_13);  // GPIO32 (X), GPIO33 (Y), GPIO13 (Z)

    // components/web_server/web_server.c
    // * ======================================================================================================
    // * [!] Voir l'exemple de la station Wi-Fi avant.
    // * ======================================================================================================
    // wifi_init();
    // esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &webserver_event_handler, NULL, NULL);
    // esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &webserver_event_handler, NULL, NULL);
    // wifi_start(WIFI_SSID, WIFI_PASSWORD);

    // components/http_client/http_client.c
    // * ======================================================================================================
    // * [!] Voir l'exemple de la station Wi-Fi avant.
    // * ======================================================================================================
    // wifi_init();
    // esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &http_client_event_handler, NULL, NULL);
    // esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &http_client_event_handler, NULL, NULL);
    // wifi_start(WIFI_SSID, WIFI_PASSWORD);

    // components/mqtt_aio/mqtt_aio.c
    // * ======================================================================================================
    // * [!] Voir l'exemple de la station Wi-Fi avant.
    // * ======================================================================================================

    // Subscribe
    // wifi_init();
    // esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &mqtt_subscribe_event_handler, NULL, NULL);
    // esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &mqtt_subscribe_event_handler, NULL, NULL);
    // wifi_start(WIFI_SSID, WIFI_PASSWORD);

    // Publish
    // wifi_init();
    // esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &mqtt_publish_event_handler, NULL, NULL);
    // esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &mqtt_publish_event_handler, NULL, NULL);
    // wifi_start(WIFI_SSID, WIFI_PASSWORD);

    // components/led_matrix/led_matrix.c
    // start_demo_led_matrix_timer(GPIO_NUM_15, GPIO_NUM_4, GPIO_NUM_2);
}