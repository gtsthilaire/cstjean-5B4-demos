#include "lcd1602.h"

#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>

static const char *TAG = "demos/20-lcd1602";

#define LCD_SDA_GPIO GPIO_NUM_13
#define LCD_SCL_GPIO GPIO_NUM_14
#define LCD_I2C_ADDR 0x27 // Souvent 0x27 ou 0x3F selon le module

static void lcd1602_task(void *arg)
{
    lcd1602_t *lcd = (lcd1602_t *)arg;

    while (1) {
        // 1. Texte statique sur les deux lignes
        ESP_LOGI(TAG, "Ligne 1 et 2");
        lcd1602_clear(lcd);
        lcd1602_set_cursor(lcd, 0, 0); // Ligne 0, Colonne 0
        lcd1602_print(lcd, "Super test !");

        lcd1602_set_cursor(lcd, 1, 0); // Ligne 1, Colonne 0

        int random_value = esp_random() % 100; // valeur random pour tester
        char buf[LCD1602_COLS];
        snprintf(buf, sizeof(buf), "Valeur=%d", random_value);
        lcd1602_print(lcd, buf);

        vTaskDelay(pdMS_TO_TICKS(2000));

        // 2. Défilement matériel : on écrit tout le message une seule fois (l'écran garde en mémoire
        // plus de caractères qu'il n'en affiche), puis on décale la fenêtre d'affichage avec scroll_left.
        // Attention : ça décale les deux lignes en même temps.
        ESP_LOGI(TAG, "Défilement");
        char msg[] = "Super test plus long !";
        int len = strlen(msg);
        int steps = len > LCD1602_COLS ? (len - LCD1602_COLS) : 0; // Décalages nécessaires pour voir la fin du message

        lcd1602_clear(lcd);
        lcd1602_set_cursor(lcd, 0, 0);
        lcd1602_print(lcd, msg);

        for (int pos = 0; pos < steps; pos++) {
            lcd1602_scroll_left(lcd);
            vTaskDelay(pdMS_TO_TICKS(300));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));

        // 3. Défilement logiciel : à chaque pas, on réécrit une fenêtre de 16 caractères du message.
        // C'est plus de travail, mais ça permet de faire défiler une seule ligne et de garder l'autre fixe.
        ESP_LOGI(TAG, "Ligne 1 et 2 - Défilement");
        lcd1602_clear(lcd);

        lcd1602_set_cursor(lcd, 1, 0);
        lcd1602_print(lcd, "ligne fixe");

        for (int pos = 0; pos < steps + 1; pos++) {
            lcd1602_set_cursor(lcd, 0, 0);
            char window[LCD1602_COLS + 1]; // +1 pour le '\0'
            strncpy(window, &msg[pos], LCD1602_COLS);
            window[LCD1602_COLS] = '\0';
            lcd1602_print(lcd, window);
            vTaskDelay(pdMS_TO_TICKS(300));
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void start_demo_20_lcd1602(void)
{
    static lcd1602_t lcd;
    lcd = lcd1602_init(LCD_SDA_GPIO, LCD_SCL_GPIO, LCD_I2C_ADDR);

    xTaskCreate(lcd1602_task, "20-lcd1602_task", 2048, &lcd, 1, NULL);
}
