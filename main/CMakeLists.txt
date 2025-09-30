idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_adc esp_event esp_netif led push_button led_bar led_pwm led_rgb 
                    led_pixel buzzer_active buzzer_passive potentiometer 
                    touch photoresistor thermistor lcd1602 servo joystick
                    wifi_ap wifi_station)
