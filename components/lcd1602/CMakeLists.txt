idf_component_register(SRCS "lcd1602.c"
                        INCLUDE_DIRS "include"
                        REQUIRES esp_driver_gpio esp_driver_i2c)
