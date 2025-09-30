#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"


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
#include "wifi_ap.h"
#include "wifi_station.h"

static const char* TAG = "exemples";

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
    // wifi_start("MON-POINT-D-ACCES", "monmotdepasse");
    
    // * ======================================================================================================
    // * [!] Comme pour le WiFi en mode station, les handlers doivent être enregistrés après l'initialisation
    // * de la pile TCP/IP et la création de la boucle d'événements. La mécanique actuelle ne nous permet pas
    // * d'avoir les deux modes (station et AP) en même temps. Il faudrait modifier la logique pour permettre
    // * d'initialiser la pile TCP/IP et la boucle d'événements qu'une seule fois.
    // * ======================================================================================================

    // components/wifi_ap/wifi_ap.c
    wifi_ap_init();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &ap_event_handler, NULL, NULL);
    wifi_ap_start("MON-POINT-D-ACCES", "monmotdepasse", 1, 4);
}