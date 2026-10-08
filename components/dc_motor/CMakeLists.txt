idf_component_register(SRCS "dc_motor.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_ledc)
