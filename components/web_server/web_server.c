/**
 * Serveur web local : on ouvre http://<IP de l'ESP32>/ dans un navigateur (sur le même réseau) pour afficher
 * une valeur mise à jour chaque seconde.
 *
 * Deux routes sont enregistrées :
 * - GET /      : la page HTML (avec un peu de JavaScript qui interroge /data chaque seconde)
 * - GET /data  : la valeur courante, au format JSON (ex : {"value": 23.45})
 *
 * La valeur est fournie par une fonction de l'application (le « data provider ») : le serveur ne sait pas d'où elle
 * vient (capteur, calcul, valeur aléatoire, ...). C'est un exemple de callback (pointeur de fonction).
 *
 * Comme il n'y a qu'un seul serveur, il n'y a pas de web_server_t : l'état est gardé dans des variables static.
 *
 * Le code est inspiré de l'exemple officiel « http_server/simple ».
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/http_server/simple/main/main.c
 */

#include "web_server.h"
#include <stdio.h>
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "components/web_server";

static httpd_handle_t s_server = NULL;

static web_server_data_provider_t s_data_provider = NULL;

// La page HTML envoyée au navigateur. Chaque ligne de texte C correspond à une ligne de la page.
static const char s_html[] =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "  <meta charset='utf-8'>\n"
    "  <meta name='viewport' content='width=device-width, initial-scale=1'>\n"
    "  <title>ESP32</title>\n"
    "  <style>\n"
    "    body { font-family: sans-serif; text-align: center; margin: 2rem; }\n"
    "    #value { font-size: 3rem; }\n"
    "  </style>\n"
    "</head>\n"
    "<body>\n"
    "  <h1>Valeur</h1>\n"
    "  <div id='value'>--</div>\n"
    "  <script>\n"
    // Toutes les secondes, on demande la valeur à l'ESP32 (route /data) et on l'affiche
    "    async function update() {\n"
    "      const response = await fetch('/data');\n"
    "      const data = await response.json();\n"
    "      document.getElementById('value').innerText = data.value.toFixed(2);\n"
    "    }\n"
    "    setInterval(update, 1000);\n"
    "  </script>\n"
    "</body>\n"
    "</html>\n";

// Route : GET /
static esp_err_t root_get(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store"); // le navigateur ne garde pas d'ancienne version en mémoire
    httpd_resp_send(req, s_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Route : GET /data
static esp_err_t data_get(httpd_req_t *req)
{
    // On demande la valeur à l'application (0 si aucune fonction n'a été choisie)
    float value = s_data_provider != NULL ? s_data_provider() : 0.0f;

    char buf[32];
    snprintf(buf, sizeof(buf), "{\"value\": %.2f}", value);

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store"); // le navigateur ne garde pas d'ancienne valeur en mémoire
    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void web_server_set_data_provider(web_server_data_provider_t provider)
{
    s_data_provider = provider;
}

void web_server_start(void)
{
    if (s_server != NULL) {
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_start(&s_server, &config);

    // Enregistrement des routes
    const httpd_uri_t root = { .uri = "/", .method = HTTP_GET, .handler = root_get };
    httpd_register_uri_handler(s_server, &root);

    const httpd_uri_t data = { .uri = "/data", .method = HTTP_GET, .handler = data_get };
    httpd_register_uri_handler(s_server, &data);

    ESP_LOGI(TAG, "Serveur web démarré");
}

void web_server_stop(void)
{
    if (s_server == NULL) {
        return;
    }

    httpd_stop(s_server);
    s_server = NULL;

    ESP_LOGI(TAG, "Serveur web arrêté");
}
