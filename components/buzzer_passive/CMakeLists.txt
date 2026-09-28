idf_component_register(SRCS "buzzer_passive.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_ledc)
