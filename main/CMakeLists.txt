idf_component_register(SRCS "main.c"
                             "demos/01-led.c"
                             "demos/02-push_button.c"
                             "demos/20-lcd1602.c"
                        INCLUDE_DIRS "."
                        REQUIRES led push_button lcd1602)
