idf_component_register(SRCS "ir_remote.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_rmt freertos)
