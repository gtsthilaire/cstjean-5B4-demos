/**
 * Une télécommande infrarouge envoie des « éclats » de lumière invisible. Le récepteur les transforme en impulsions
 * électriques (état bas = lumière reçue).
 *
 * Le protocole NEC (utilisé par la télécommande du kit) code chaque bit par des DURÉES :
 *   - début de trame : 9 ms de lumière, puis 4.5 ms de pause ;
 *   - bit 0 : 0.56 ms de lumière, puis 0.56 ms de pause ;
 *   - bit 1 : 0.56 ms de lumière, puis 1.69 ms de pause (pause plus longue).
 * Une trame contient 32 bits : l'adresse de la télécommande et la commande (la touche).
 *
 * Mesurer ces durées à la main serait délicat. L'ESP32 a un périphérique fait pour ça : le RMT (Remote Control).
 * Il mesure lui-même la durée de chaque impulsion et nous donne la liste quand la trame est terminée.
 *
 * Le RMT travaille tout seul, en parallèle de notre programme. Quand une trame est terminée, il déclenche
 * une « interruption » : le processeur arrête ce qu'il était en train de faire, exécute la fonction
 * on_receive_done, puis reprend exactement là où il était.
 *
 * Pendant ce temps, le reste du programme attend : on_receive_done doit donc être très courte.
 * Elle lève seulement un drapeau (frame_received) que ir_remote_read surveille pour savoir que
 * la trame est prête à être décodée.
 *
 * Inspiré de l'exemple ESP-IDF peripherals/rmt/ir_nec_transceiver (en version simplifiée).
 */

#include "ir_remote.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "components/ir_remote";

#define IR_RESOLUTION_HZ 1000000 // 1 MHz : les durées mesurées sont en microsecondes
#define IR_MARGIN_US 200         // Tolérance sur les durées (la télécommande n'est pas parfaitement précise)

#define NEC_START_LIGHT_US 9000
#define NEC_START_PAUSE_US 4500
#define NEC_BIT_LIGHT_US 560
#define NEC_BIT_0_PAUSE_US 560
#define NEC_BIT_1_PAUSE_US 1690
#define NEC_BITS 32

// Mises à jour par on_receive_done (pendant l'interruption), lues par ir_remote_read.
// volatile : le compilateur doit relire leur valeur à chaque fois, car elles changent « en cachette ».
static volatile bool frame_received = false;
static volatile size_t received_count = 0; // nombre d'impulsions dans la trame reçue

// Appelée automatiquement par le RMT quand une trame est terminée (c'est l'interruption).
// Le reste du programme attend pendant ce temps : on fait le strict minimum, c'est-à-dire
// lever le drapeau pour prévenir ir_remote_read.
static bool on_receive_done(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
    received_count = edata->num_symbols;
    frame_received = true;
    return false;
}

// Vérifie qu'une durée mesurée est proche de la durée attendue.
static bool is_close(uint32_t measured_us, uint32_t expected_us)
{
    return measured_us > expected_us - IR_MARGIN_US && measured_us < expected_us + IR_MARGIN_US;
}

ir_remote_t ir_remote_init(gpio_num_t gpio)
{
    ESP_LOGI(TAG, "Configuration du GPIO %d en réception infrarouge (RMT)", gpio);

    ir_remote_t ir_remote = { 0 };

    rmt_rx_channel_config_t channel_config = {
        .gpio_num = gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = IR_RESOLUTION_HZ,
        .mem_block_symbols = IR_REMOTE_MAX_SYMBOLS,
    };
    rmt_new_rx_channel(&channel_config, &ir_remote.channel);

    // Indique au RMT d'appeler on_receive_done à la fin d'une trame
    rmt_rx_event_callbacks_t callbacks = {
        .on_recv_done = on_receive_done,
    };
    rmt_rx_register_event_callbacks(ir_remote.channel, &callbacks, NULL);

    rmt_enable(ir_remote.channel);

    return ir_remote;
}

bool ir_remote_read(ir_remote_t *ir_remote, uint32_t *code)
{
    if (ir_remote == NULL || code == NULL) {
        return false;
    }

    frame_received = false;

    // Démarre l'écoute (rmt_receive retourne tout de suite, sans attendre la trame). Les impulsions plus courtes que 1.25 µs sont des parasites,
    // et une pause de plus de 12 ms veut dire que la trame est terminée.
    rmt_receive_config_t receive_config = {
        .signal_range_min_ns = 1250,
        .signal_range_max_ns = 12000000,
    };
    rmt_receive(ir_remote->channel, ir_remote->symbols, sizeof(ir_remote->symbols), &receive_config);

    // Attend que on_receive_done lève le drapeau, c'est-à-dire qu'une trame soit terminée.
    // vTaskDelay laisse le processeur aux autres tâches pendant l'attente.
    while (!frame_received) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Une trame complète = le début + 32 bits (+ une petite impulsion de fin)
    if (received_count < NEC_BITS + 1) {
        return false; // trop court : touche maintenue enfoncée (trame de répétition) ou parasite
    }

    // Chaque « symbole » contient une durée de lumière (duration0) et la pause qui suit (duration1)
    rmt_symbol_word_t *symbol = ir_remote->symbols;

    if (!is_close(symbol->duration0, NEC_START_LIGHT_US) || !is_close(symbol->duration1, NEC_START_PAUSE_US)) {
        return false; // pas un début de trame NEC
    }
    symbol++;

    uint32_t value = 0;
    for (int i = 0; i < NEC_BITS; i++, symbol++) {
        if (!is_close(symbol->duration0, NEC_BIT_LIGHT_US)) {
            return false;
        }

        // La durée de la pause indique si c'est un 0 ou un 1
        int bit;
        if (is_close(symbol->duration1, NEC_BIT_1_PAUSE_US)) {
            bit = 1;
        } else if (is_close(symbol->duration1, NEC_BIT_0_PAUSE_US)) {
            bit = 0;
        } else {
            return false;
        }

        // On ajoute les bits dans l'ordre de réception : on obtient les mêmes codes que dans le tutoriel
        value = (value << 1) | bit;
    }

    *code = value;
    return true;
}
