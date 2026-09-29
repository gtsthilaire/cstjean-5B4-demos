#ifndef WEB_SERVER_H
#define WEB_SERVER_H

// Pointeur vers une fonction qui retourne la valeur à afficher dans la page web.
// Le serveur ne sait pas d'où vient la valeur (capteur, calcul, ...) : c'est l'application qui la fournit.
typedef float (*web_server_data_provider_t)(void);

// Démarre le serveur web (port 80). Ne fait rien s'il est déjà démarré.
void web_server_start(void);

// Arrête le serveur web.
void web_server_stop(void);

// Choisit la fonction appelée pour obtenir la valeur affichée par la page web (route GET /data).
void web_server_set_data_provider(web_server_data_provider_t provider);

#endif // WEB_SERVER_H
