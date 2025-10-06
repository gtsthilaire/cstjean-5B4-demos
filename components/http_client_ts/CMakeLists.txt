idf_component_register(
    SRCS "http_client_ts.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_http_client esp-tls
)