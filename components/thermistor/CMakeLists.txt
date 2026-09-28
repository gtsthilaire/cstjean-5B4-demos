idf_component_register(SRCS "thermistor.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_adc)
