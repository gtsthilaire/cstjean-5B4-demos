idf_component_register(SRCS "led_rgb.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio led_pwm)
