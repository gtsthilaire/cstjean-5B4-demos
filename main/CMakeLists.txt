idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_adc led push_button led_bar led_pwm led_rgb led_pixel buzzer_active buzzer_passive potentiometer touch)
