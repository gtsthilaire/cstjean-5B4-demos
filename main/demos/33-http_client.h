/**
 * Démonstration d'un client HTTP qui envoie des données à ThingSpeak.
 * Une fois l'ESP32 connecté au Wi-Fi, deux valeurs (aléatoires entre 20 et 30) sont envoyées toutes les 20 secondes
 * dans les champs field1 et field2 du canal ThingSpeak. Les graphiques du canal se mettent à jour en ligne.
 *
 * L'envoi démarre la première fois que l'ESP32 obtient une adresse IP. Si le Wi-Fi est coupé, les envois échouent,
 * puis reprennent tout seuls à la reconnexion. Voir components/thingspeak.
 *
 * Configuration : idf.py menuconfig → Configuration des exemples → SSID, mot de passe et clé d'API ThingSpeak
 * (Write API Key du canal).
 *
 * Pas de chapitre équivalent dans le tutoriel Freenove (le plus proche est le chapitre 33 - TCP/IP).
 * Le code est inspiré de l'exemple officiel « esp_http_client » d'ESP-IDF.
 */

void start_demo_33_http_client(void);
