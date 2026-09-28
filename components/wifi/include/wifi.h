#ifndef WIFI_H
#define WIFI_H

// À noter : les noms wifi_init() et wifi_station_start() sont déjà utilisés en interne par la bibliothèque Wi-Fi
// d'Espressif. D'où wifi_setup(), wifi_start_station() et wifi_start_ap().

// Initialise la mémoire NVS, la pile TCP/IP, la boucle d'événements et le Wi-Fi. Peut être appelée plusieurs fois
// (seul le premier appel fait quelque chose). Les gestionnaires d'événements de l'application doivent être
// enregistrés APRÈS cet appel (la boucle d'événements doit exister) et AVANT wifi_start_station / wifi_start_ap.
void wifi_setup(void);

// Se connecte au réseau Wi-Fi donné (mode station). En cas de déconnexion, une reconnexion est tentée automatiquement.
// Un mot de passe vide ou NULL permet de se connecter à un réseau ouvert.
void wifi_start_station(const char *ssid, const char *password);

// Crée un point d'accès Wi-Fi (SoftAP). Un mot de passe vide ou NULL crée un réseau ouvert (sinon 8 caractères minimum).
void wifi_start_ap(const char *ssid, const char *password, int channel, int max_conn);

#endif // WIFI_H
