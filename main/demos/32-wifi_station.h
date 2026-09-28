/**
 * Démonstration de la connexion à un réseau Wi-Fi (mode station).
 * L'ESP32 se connecte au réseau configuré dans menuconfig, puis la démo affiche les changements d'état
 * (connexion en cours, connecté avec l'adresse IP, déconnecté).
 *
 * Configuration : idf.py menuconfig → Configuration des exemples → SSID et mot de passe du réseau Wi-Fi.
 *
 * La démo montre comment l'application peut réagir aux événements du Wi-Fi : elle enregistre son propre
 * gestionnaire d'événements, en plus de celui du composant wifi (qui s'occupe de la connexion et de la reconnexion).
 *
 * Tutoriel original : Chapitre 32 - WiFi Working Modes (Freenove), projet 32.1 (Station mode)
 */

void start_demo_32_wifi_station(void);
