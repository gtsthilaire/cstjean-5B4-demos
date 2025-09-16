idf_component_register(SRCS "thermistor.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos esp_adc)
