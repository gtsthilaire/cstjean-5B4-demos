#ifndef ADAFRUIT_IO_H
#define ADAFRUIT_IO_H

#include <stddef.h>

// Pointeur vers une fonction appelée à chaque connexion (et reconnexion) au broker.
typedef void (*adafruit_io_connected_cb_t)(void);

// Pointeur vers une fonction appelée à chaque message reçu. Elle reçoit le topic et le payload, avec leurs
// longueurs (ils ne se terminent pas par '\0').
typedef void (*adafruit_io_message_cb_t)(const char *topic, size_t topic_len, const char *payload, size_t payload_len);

// Choisit la fonction appelée à chaque connexion (et reconnexion) au broker. À appeler avant adafruit_io_start.
void adafruit_io_set_connected_callback(adafruit_io_connected_cb_t callback);

// Choisit la fonction appelée à chaque message reçu. À appeler avant adafruit_io_start.
void adafruit_io_set_message_callback(adafruit_io_message_cb_t callback);

// Se connecte au broker d'Adafruit IO avec le nom d'utilisateur et la clé donnés. Ne fait rien si c'est déjà démarré.
// La connexion n'est jamais arrêtée : en cas de coupure, le client se reconnecte tout seul.
// Les chaînes user et key doivent rester valides tout le temps (ex : CONFIG_EXEMPLES_AIO_USER).
void adafruit_io_start(const char *user, const char *key);

// Publie un texte sur un feed. Ne fait rien tant que la connexion n'est pas établie.
void adafruit_io_publish_text(const char *feed, const char *payload);

// Publie un nombre à virgule sur un feed (converti en texte avec deux décimales).
void adafruit_io_publish_float(const char *feed, float value);

// S'abonne à un feed : les messages reçus seront transmis à la fonction choisie avec adafruit_io_set_message_callback.
void adafruit_io_subscribe(const char *feed);

#endif // ADAFRUIT_IO_H
