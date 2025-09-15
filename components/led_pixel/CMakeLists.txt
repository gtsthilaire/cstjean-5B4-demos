idf_component_register(SRCS "led_pixel.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver freertos
                    PRIV_REQUIRES led_strip)