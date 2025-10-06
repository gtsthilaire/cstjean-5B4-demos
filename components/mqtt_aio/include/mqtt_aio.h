#ifndef MQTT_AIO_H
#define MQTT_AIO_H

#include <stddef.h>
#include <stdio.h>

// On créé un type mqtt_msg_cb_t qui est un pointeur vers une fonction.
// Cette fonction prend en paramètre un pointeur vers un char (le topic),
// un pointeur vers un char (le payload) et une taille (la longueur du payload).
typedef void (*mqtt_msg_cb_t)(const char* topic, size_t topic_len, const char* payload, size_t payload_len);

void mqtt_start(const char* user, const char* key, const char* feed);
void mqtt_stop(void);

// On va publier un message. Le payload est une chaîne de caractères.
// retained indique si le message doit être retenu par le broker ou non.
void mqtt_publish_text(const char* payload, int retained);

// Le payload est un float. On le convertit en chaîne de caractères
// avec deux décimales avant de le publier. On doit envoyer une chaîne
// de caractères, car le protocole MQTT ne gère pas les types de données.
static inline void mqtt_publish_float(float f, int retained) {
    char buf[32]; 
    snprintf(buf, sizeof(buf), "%.2f", f);
    mqtt_publish_text(buf, retained);
}

void mqtt_set_message_callback(mqtt_msg_cb_t cb);

#endif
