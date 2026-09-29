#ifndef THINGSPEAK_H
#define THINGSPEAK_H

#include <stdint.h>

// Pointeur vers une fonction qui retourne la valeur à envoyer. Même principe que web_server_data_provider_t
// (voir components/web_server) : c'est l'application qui fournit la valeur.
typedef float (*thingspeak_data_provider_t)(void);

// Choisit les fonctions qui fournissent les valeurs envoyées dans les champs field1 et field2 du canal ThingSpeak.
void thingspeak_set_data_provider(thingspeak_data_provider_t provider1, thingspeak_data_provider_t provider2);

// Démarre une tâche qui envoie les données à ThingSpeak toutes les period_ms millisecondes (15 s minimum pour
// ThingSpeak). Ne fait rien si elle est déjà démarrée. La tâche n'est jamais arrêtée.
// La chaîne api_key doit rester valide tout le temps (ex : CONFIG_EXEMPLES_TS_API_KEY).
void thingspeak_start(const char *api_key, uint32_t period_ms);

#endif // THINGSPEAK_H
