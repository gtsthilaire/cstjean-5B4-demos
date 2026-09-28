idf_component_register(SRCS "led_matrix.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_timer)
