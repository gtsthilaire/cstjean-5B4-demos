/**
 * Client HTTP qui envoie des données à ThingSpeak (https://thingspeak.mathworks.com), un service en ligne qui
 * enregistre des valeurs et les affiche sous forme de graphiques.
 *
 * Pour ajouter une valeur, il suffit de faire une requête HTTP GET avec la clé d'écriture du canal et les valeurs :
 *   https://api.thingspeak.com/update?api_key=XXXXXXXXXX&field1=0.0&field2=0.0
 *
 * La connexion est en HTTPS : le certificat du serveur est vérifié avec le paquet de certificats racines
 * inclus dans ESP-IDF (esp_crt_bundle_attach).
 *
 * La tâche d'envoi n'est jamais arrêtée. Si on la supprimait pendant une requête, la mémoire de la connexion ne
 * serait pas libérée. Donc, sans Wi-Fi, l'envoi échoue simplement, puis reprend tout seul à la reconnexion.
 *
 * Comme il n'y a qu'une seule tâche d'envoi, il n'y a pas de thingspeak_t : l'état est gardé dans des variables static.
 *
 * Le code est inspiré de l'exemple officiel « esp_http_client ».
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/esp_http_client/main/esp_http_client_example.c
 */

#include "thingspeak.h"
#include <stdio.h>
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "components/thingspeak";

static thingspeak_data_provider_t s_data_provider1 = NULL;
static thingspeak_data_provider_t s_data_provider2 = NULL;

static TaskHandle_t s_task = NULL;

static uint32_t s_period_ms = 20000;
static const char *s_api_key = NULL;

void thingspeak_set_data_provider(thingspeak_data_provider_t provider1, thingspeak_data_provider_t provider2)
{
    s_data_provider1 = provider1;
    s_data_provider2 = provider2;
}

// Envoie les deux valeurs à ThingSpeak et affiche le résultat.
static void send_update(float value1, float value2)
{
    // Construire l'URL avec les données à envoyer.
    char url[256];
    snprintf(url, sizeof(url), "https://api.thingspeak.com/update?api_key=%s&field1=%.2f&field2=%.2f",
             s_api_key, value1, value2);

    esp_http_client_config_t http_client_config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 5000,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t http_client_handle = esp_http_client_init(&http_client_config);

    // Effectuer la requête, puis afficher le code de statut HTTP (200 = reçu par ThingSpeak).
    // Sans Wi-Fi, la requête échoue : le statut n'est pas 200, et on réessaiera au prochain envoi.
    // À noter : ThingSpeak répond aussi 200 quand il ignore une mise à jour trop rapide (moins de 15 s).
    esp_http_client_perform(http_client_handle);
    int status = esp_http_client_get_status_code(http_client_handle);
    ESP_LOGI(TAG, "Envoi field1=%.2f field2=%.2f → statut HTTP %d", value1, value2, status);

    esp_http_client_cleanup(http_client_handle); // Libère la mémoire de la connexion
}

static void thingspeak_task(void *arg)
{
    TickType_t last = xTaskGetTickCount();

    while (1) {
        // On demande les valeurs à l'application (0 si aucune fonction n'a été choisie)
        float value1 = s_data_provider1 != NULL ? s_data_provider1() : 0.0f;
        float value2 = s_data_provider2 != NULL ? s_data_provider2() : 0.0f;

        send_update(value1, value2);

        // vTaskDelayUntil garde une période régulière, même si la requête a pris du temps
        vTaskDelayUntil(&last, pdMS_TO_TICKS(s_period_ms));
    }
}

void thingspeak_start(const char *api_key, uint32_t period_ms)
{
    if (s_task != NULL) {
        return; // Déjà démarrée
    }

    s_api_key = api_key;
    s_period_ms = period_ms;

    xTaskCreate(thingspeak_task, "thingspeak_task", 4096, NULL, 1, &s_task);
    ESP_LOGI(TAG, "Envoi à ThingSpeak démarré (toutes les %lu ms)", (unsigned long)s_period_ms);
}
