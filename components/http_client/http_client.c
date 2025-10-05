/**
 * Démonstration de l'utilisation d'un client HTTP.
 * On envoie des données à ThingSpeak.
 * 
 * Le code est inspiré de l'exemple officiel "esp_http_client".
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/esp_http_client/main/esp_http_client_example.c
 */

#include "http_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "http_client.c";

static data_provider_t s_data_provider1 = NULL, s_data_provider2 = NULL;

static TaskHandle_t s_task = NULL;

static uint32_t s_period_ms = 20000;
static char s_api_key[32] = {0};

void http_client_set_data_provider(data_provider_t provider1, data_provider_t provider2) 
{ 
    s_data_provider1 = provider1; 
    s_data_provider2 = provider2; 
}

static esp_err_t ts_http_update(float value1, float value2) 
{
    // Construire l'URL avec les données à envoyer.
    // http://api.thingspeak.com/update?api_key=XXXXXXXXXX&field1=0.0&field2=0.0
    // On devrait idéalement utiliser HTTPS. À voir plus tard.
    char url[512];
    snprintf(url, sizeof(url), "http://api.thingspeak.com/update?api_key=%s&field1=%.2f&field2=%.2f",
                     s_api_key, value1, value2);

    esp_http_client_config_t http_client_config = {.url = url, .method = HTTP_METHOD_GET, .timeout_ms = 5000};

    esp_http_client_handle_t http_client_handle = esp_http_client_init(&http_client_config);

    // Effectuer la requête et on garde le code de statut HTTP et le corps de la réponse.
    esp_err_t err = esp_http_client_perform(http_client_handle);
    
    int status = (err == ESP_OK) ? esp_http_client_get_status_code(http_client_handle) : -1;
    
    esp_http_client_cleanup(http_client_handle);

    if (err != ESP_OK || status != 200) {
        ESP_LOGE(TAG, "HTTP err=%s status=%d", esp_err_to_name(err), status);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "OK (value1=%.2f value2=%.2f)", value1, value2);

    return ESP_OK;
}

static void http_client_task(void* arg) 
{
    TickType_t last = xTaskGetTickCount();

    while (1) {
        // Obtenir les données à envoyer. Si un provider n'est pas prêt, on n'envoie rien.
        float value1 = 0, value2 = 0;
        bool is_valid_value1 = s_data_provider1 && s_data_provider1(&value1);
        bool is_valid_value2 = s_data_provider2 && s_data_provider2(&value2);

        if (is_valid_value1 && is_valid_value2) {
            ts_http_update(value1, value2); // Envoyer les données à ThingSpeak
        }

        vTaskDelayUntil(&last, pdMS_TO_TICKS(s_period_ms));
    }
}

void start_demo_http_client_task(char *api_key, uint32_t period_ms) 
{
    if (s_task) return;
    if (period_ms) s_period_ms = period_ms;

    strncpy(s_api_key, api_key, sizeof(s_api_key) - 1);
    s_api_key[sizeof(s_api_key) - 1] = '\0';

    xTaskCreate(http_client_task, "http_client_task", 4096, NULL, 5, &s_task);
}

void stop_demo_http_client_task(void) 
{
    if (!s_task) return;
    
    vTaskDelete(s_task);
    s_task = NULL;
}