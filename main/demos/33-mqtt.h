/**
 * Démonstration de MQTT avec Adafruit IO.
 * Une fois l'ESP32 connecté au Wi-Fi puis au broker MQTT, la démo :
 * - publie une valeur (aléatoire entre 20 et 30) sur le feed « demo-5b4 » toutes les 5 secondes;
 * - s'abonne au même feed et affiche tous les messages reçus.
 * On voit donc passer chaque valeur publiée (le broker nous la renvoie), ainsi que les valeurs envoyées
 * depuis le tableau de bord d'Adafruit IO.
 *
 * Les deux parties sont indépendantes : un appareil peut seulement publier (ex : un capteur qui envoie ses mesures)
 * ou seulement s'abonner (ex : une lampe qui attend des ordres). Voir les commentaires PUBLISH / SUBSCRIBE dans
 * 33-mqtt.c pour savoir quoi retirer.
 *
 * Le feed « demo-5b4 » doit être créé au préalable sur Adafruit IO.
 *
 * La connexion au broker démarre la première fois que l'ESP32 obtient une adresse IP. En cas de coupure, le client
 * MQTT se reconnecte tout seul (et se réabonne au feed). Voir components/adafruit_io.
 *
 * Configuration : idf.py menuconfig → Configuration des exemples → SSID, mot de passe, utilisateur et clé Adafruit IO.
 *
 * Pas de chapitre équivalent dans le tutoriel Freenove (le plus proche est le chapitre 33 - TCP/IP).
 * Le code est inspiré de l'exemple officiel « mqtt/tcp » d'ESP-IDF.
 */

void start_demo_33_mqtt(void);
