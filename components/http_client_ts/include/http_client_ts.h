#ifndef HTTP_CLIENT_TS_H
#define HTTP_CLIENT_TS_H

#include <stdbool.h>
#include <stdint.h> 

// * ======================================================================================================
// * [!] Voir l'exemple du web_server.
// * ======================================================================================================
typedef bool (*data_provider_t)(float* out_data);

void start_demo_http_client_task(char *api_key, uint32_t period_ms);
void stop_demo_http_client_task(void);
void http_client_set_data_provider(data_provider_t provider1, data_provider_t provider2);

#endif
