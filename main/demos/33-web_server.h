/**
 * Démonstration d'un serveur web local.
 * Une fois l'ESP32 connecté au Wi-Fi, ouvrez l'adresse affichée dans la console (http://<IP>/) dans un navigateur
 * branché sur le même réseau : la page affiche une valeur (aléatoire entre 20 et 30) mise à jour chaque seconde.
 * Tant que la page est ouverte, la console affiche une ligne à chaque requête (une par seconde).
 *
 * Le serveur est démarré quand l'ESP32 obtient une adresse IP et arrêté quand il perd la connexion.
 * Dans un vrai projet, la valeur viendrait d'un capteur (ex : components/thermistor).
 *
 * Configuration : idf.py menuconfig → Configuration des exemples → SSID et mot de passe du réseau Wi-Fi.
 *
 * Pas de chapitre équivalent dans le tutoriel Freenove (le plus proche est le chapitre 33 - TCP/IP).
 * Le code est inspiré de l'exemple officiel « http_server/simple » d'ESP-IDF.
 */

void start_demo_33_web_server(void);
