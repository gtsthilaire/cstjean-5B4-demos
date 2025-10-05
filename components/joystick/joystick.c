/**
 * Démonstration de l'utilisation d'un joystick.
 * Le joystick permet de mesurer deux axes (X, Y) et un bouton (Z).
 * Les axes X et Y sont lus via des entrées analogiques, tandis que le bouton Z est une entrée numérique.
 *
 * =======================================================================================
 * [!] Voir le component potentiometer pour une explication plus détaillée sur le ADC/DAC.
 * =======================================================================================
 * 
 * Tutoriel original : Chapitre 14 - Joystick (Freenove)
 */

#include "joystick.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "joystick.c";

static adc_channel_t s_joystick_x_channel = -1;
static adc_channel_t s_joystick_y_channel = -1;
static gpio_num_t s_joystick_z_gpio = -1;

static adc_oneshot_unit_handle_t s_adc_handle;

static void joystick_init(adc_channel_t channel_x, adc_channel_t channel_y, gpio_num_t gpio_z)
{
    s_joystick_x_channel = channel_x;
    s_joystick_y_channel = channel_y;
    s_joystick_z_gpio = gpio_z;

    adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_new_unit(&unit_config, &s_adc_handle);

    adc_oneshot_chan_cfg_t ch_config = {
        .bitwidth = ADC_BITWIDTH_12,     // valeurs 0..4095
        .atten = ADC_ATTEN_DB_11,        // plage ~0..3.3V
    };
    adc_oneshot_config_channel(s_adc_handle, s_joystick_x_channel, &ch_config);
    adc_oneshot_config_channel(s_adc_handle, s_joystick_y_channel, &ch_config);

    gpio_reset_pin(s_joystick_z_gpio);
    gpio_set_direction(s_joystick_z_gpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(s_joystick_z_gpio, GPIO_PULLUP_ONLY);
}

static void joystick_task(void *arg)
{
    while (1) {
        // ==================================================================================
        // [!] Pour un résultat plus précis, on pourrait :
        // - Faire une moyenne de plusieurs lectures
        // - Ajouter un filtre pour lisser les variations rapides
        // - Calibrer les valeurs pour compenser les imperfections du joystick
        // - Définir une zone morte pour éviter des variations quand le joystick est au repos
        // - Mettre un debounce sur le bouton Z pour éviter les rebonds
        // ==================================================================================

        int adc_raw_x = 0;
        int adc_raw_y = 0;
        adc_oneshot_read(s_adc_handle, s_joystick_x_channel, &adc_raw_x);
        adc_oneshot_read(s_adc_handle, s_joystick_y_channel, &adc_raw_y);
        int button_state = gpio_get_level(s_joystick_z_gpio);

        ESP_LOGI(TAG, "X-axis: %4d, Y-axis: %4d, Z-axis: %d", adc_raw_x, adc_raw_y, button_state);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void start_demo_joystick_task(adc_channel_t channel_x, adc_channel_t channel_y, gpio_num_t gpio_z)
{
    joystick_init(channel_x, channel_y, gpio_z);

    xTaskCreate(joystick_task, "joystick_task", 2048, NULL, 5, NULL);
}