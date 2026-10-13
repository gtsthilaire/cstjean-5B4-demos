/**
 * Démonstration de l'utilisation du capteur tactile de l'ESP32.
 * Un simple fil branché sur une broche tactile sert de capteur. La valeur mesurée est affichée dans la
 * console, et un message indique quand on touche ou relâche le fil.
 *
 * Ne pas toucher le fil au démarrage : la valeur au repos sert à calculer le seuil.
 * Voir components/touch pour le fonctionnement du capteur.
 *
 * Tutoriel original : Chapitre 10 - Touch Sensor (Freenove)
 * À noter que, contrairement au tutoriel, il n'y a pas de LED (voir la démo 01).
 */

void start_demo_10_touch(void);
