/**
 * Le DHT11 mesure la température et l'humidité et envoie le résultat sur UN SEUL fil de données.
 * Le même fil sert dans les deux sens, à tour de rôle :
 *   1. L'ESP32 met le fil à l'état bas pendant 20 ms pour « réveiller » le capteur, puis le relâche.
 *   2. Le capteur répond par une impulsion basse puis haute (« je suis là »).
 *   3. Le capteur envoie 40 bits. Chaque bit commence par un état bas, suivi d'un état haut :
 *        - état haut court (environ 27 µs) → bit 0
 *        - état haut long (environ 70 µs)  → bit 1
 *
 * Les 40 bits forment 5 octets : humidité, (décimale), température, (décimale), somme de contrôle.
 * La somme de contrôle permet de vérifier qu'aucun bit n'a été mal lu : elle doit être égale à
 * la somme des 4 premiers octets.
 */

#include "dht11.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "components/dht11";

#define DHT11_TIMEOUT_US 200   // Aucune impulsion du capteur ne dure aussi longtemps
#define DHT11_BIT_1_MIN_US 40  // État haut plus long que ça → bit 1
#define DHT11_BYTES 5

// De temps en temps, le processeur arrête ce qu'il fait pour exécuter une autre tâche urgente
// (c'est une « interruption »), puis revient. Pendant la lecture, une seule pause de quelques
// microsecondes fausserait la mesure des durées. Ce verrou sert à bloquer ces interruptions.
static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;

// Attend que le fil passe au niveau demandé. Retourne le temps attendu (µs), ou -1 si c'est trop long.
static int wait_for_level(gpio_num_t gpio, int level)
{
    int64_t start_us = esp_timer_get_time();
    while (gpio_get_level(gpio) != level) {
        if (esp_timer_get_time() - start_us > DHT11_TIMEOUT_US) {
            return -1;
        }
    }
    return esp_timer_get_time() - start_us;
}

dht11_t dht11_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en drain ouvert", gpio);

    gpio_reset_pin(gpio);
    gpio_set_direction(gpio, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(gpio, GPIO_PULLUP_ONLY);
    gpio_set_level(gpio, 1); // fil relâché

    return (dht11_t){ .gpio = gpio };
}

bool dht11_read(dht11_t *dht11, int *temperature, int *humidity)
{
    if (dht11 == NULL || temperature == NULL || humidity == NULL) {
        return false;
    }

    uint8_t data[DHT11_BYTES] = { 0 };
    bool ok = true;

    // 1. Réveil du capteur : fil à l'état bas pendant 20 ms
    gpio_set_level(dht11->gpio, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    // Début de la zone protégée (plus d'interruption). Sans elle, une pause de 20 µs pendant
    // un bit 0 (27 µs) donnerait 47 µs et on lirait un 1 par erreur.
    portENTER_CRITICAL(&s_lock);

    gpio_set_level(dht11->gpio, 1); // on relâche le fil : c'est maintenant au capteur de parler

    // 2. Réponse du capteur : il tire le fil vers le bas, puis le relâche, puis commence l'envoi
    if (wait_for_level(dht11->gpio, 0) < 0 || wait_for_level(dht11->gpio, 1) < 0 || wait_for_level(dht11->gpio, 0) < 0) {
        ok = false;
    }

    // 3. Lecture des 40 bits : c'est la durée de l'état haut qui donne la valeur du bit
    for (int i = 0; ok && i < DHT11_BYTES * 8; i++) {
        int high_us = -1;
        if (wait_for_level(dht11->gpio, 1) < 0 || (high_us = wait_for_level(dht11->gpio, 0)) < 0) {
            ok = false;
            break;
        }

        int bit = high_us > DHT11_BIT_1_MIN_US;
        data[i / 8] = (data[i / 8] << 1) | bit; // on ajoute le bit à droite de l'octet en cours
    }

    // Fin de la zone protégée : les interruptions reprennent
    portEXIT_CRITICAL(&s_lock);

    if (!ok) {
        ESP_LOGW(TAG, "Le capteur ne répond pas");
        return false;
    }

    // Vérification : la somme des 4 premiers octets doit donner le 5e
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        ESP_LOGW(TAG, "Somme de contrôle invalide");
        return false;
    }

    *humidity = data[0];
    *temperature = data[2];
    return true;
}
