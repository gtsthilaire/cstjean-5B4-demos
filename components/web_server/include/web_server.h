#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <stdbool.h> 

// On créé un type data_provider_t qui est un pointeur vers une fonction.
// Cette fonction prend en paramètre un pointeur vers un float.
// C'est dans ce paramètre que la fonction va écrire la donnée qu'elle fournit.
// La fonction retourne un bool qui indique si la donnée est valide ou non.
// Ici on travaille avec des float, mais on pourrait très bien imaginer
// un data_provider_t qui fournit des int, des structures, etc.
typedef bool (*data_provider_t)(float* out_data);

void web_server_start(void);
void web_server_stop(void);
void web_server_set_data_provider(data_provider_t provider);

#endif
