idf_component_register(SRCS "joystick.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_adc push_button)
