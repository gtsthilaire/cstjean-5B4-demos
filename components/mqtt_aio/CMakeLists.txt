idf_component_register(
    SRCS "mqtt_aio.c"
    INCLUDE_DIRS "include"
    REQUIRES mqtt esp-tls
)