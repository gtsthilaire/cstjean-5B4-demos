idf_component_register(SRCS "led_matrix.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos esp_timer)