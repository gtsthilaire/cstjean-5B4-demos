/**
 * Démonstration d'un point d'accès Wi-Fi (SoftAP).
 * L'ESP32 crée son propre réseau Wi-Fi. La démo affiche les appareils (téléphone, ordinateur, ...) qui s'y
 * connectent et s'en déconnectent.
 *
 * Configuration : idf.py menuconfig → Configuration des exemples → SSID et mot de passe du point d'accès
 * (par défaut « ESP32-DEMO » / « 12345678 » ; 8 caractères minimum pour le mot de passe).
 *
 * Tutoriel original : Chapitre 32 - WiFi Working Modes (Freenove), projet 32.2 (AP mode)
 */

void start_demo_32_wifi_ap(void);
