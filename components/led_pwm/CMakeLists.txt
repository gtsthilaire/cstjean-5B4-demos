idf_component_register(SRCS "led_pwm.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_ledc)
