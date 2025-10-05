/**
 * Démonstration de l'utilisation d'un serveur web local.
 * 
 * Le code est inspiré de l'exemple officiel "http_server/simple".
 * https://github.com/espressif/esp-idf/blob/master/examples/protocols/http_server/simple/main/main.c
 */

#include "web_server.h"
#include "esp_http_server.h"
#include "esp_log.h"

static const char *TAG = "web_server";

static httpd_handle_t s_server = NULL;

static data_provider_t s_data_provider = NULL;

static const char s_html[] =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'/>"
    "<title>TEST ESP32</title>"
    "<style>body{font-family:sans-serif;text-align:center;margin:2rem}"
    "h1{font-size:1.5rem}#data{font-size:2.5rem}</style>"
    "<script>"
    "async function update(){"
    "  try{const r=await fetch('/data',{cache:'no-store'});"
    "      if(!r.ok) throw new Error('HTTP '+r.status);"
    "      const d=await r.json();"
    "      document.getElementById('data').innerText="
    "         +(d.value?.toFixed?d.value.toFixed(2):d.value);"
    "  }catch(e){document.getElementById('data').innerText='N/A';}"
    "} setInterval(update,1000); window.onload=update;"
    "</script></head>"
    "<body><h1>Data</h1><div id='data'>--</div></body></html>";

// Route : GET /    
static esp_err_t root_get(httpd_req_t *req) 
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_send(req, s_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Route : GET /data
static esp_err_t data_get(httpd_req_t *req) 
{
    char buf[64];

    if (s_data_provider) {
        float d = 0.f;

        // On demande une donnée au fournisseur
        if (s_data_provider(&d)) {
            int n = snprintf(buf, sizeof(buf), "{\"value\": %.2f}", d);
            httpd_resp_set_type(req, "application/json; charset=utf-8");
            httpd_resp_set_hdr(req, "Cache-Control", "no-store");
            httpd_resp_send(req, buf, n);
            return ESP_OK;
        }
    }
    
    httpd_resp_set_status(req, "503 Service Unavailable");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"error\":\"no data\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// On peut changer la fonction de fourniture de données
void web_server_set_data_provider(data_provider_t provider) 
{
    s_data_provider = provider;
}

void web_server_start(void) 
{
    if (s_server) return;

    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();

    esp_err_t err = httpd_start(&s_server, &cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "web_server_start a échoué: %s", esp_err_to_name(err));
        s_server = NULL;
        return;
    }

    // Enregistrement des routes
    // Route : GET /
    const httpd_uri_t root = { .uri="/", .method=HTTP_GET, .handler=root_get };
    httpd_register_uri_handler(s_server, &root);

    // Route : GET /data
    const httpd_uri_t data = { .uri="/data", .method=HTTP_GET, .handler=data_get };
    httpd_register_uri_handler(s_server, &data);
}

void web_server_stop(void) 
{
    if (!s_server) return;
 
    httpd_stop(s_server);
    s_server = NULL;
}