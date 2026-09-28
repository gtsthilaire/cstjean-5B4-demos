idf_component_register(SRCS "touch.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_touch_sens)
