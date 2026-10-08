/**
 * Démonstration de l'utilisation d'un capteur à ultrasons (HC-SR04).
 * La distance de l'obstacle devant le capteur est affichée dans la console.
 *
 * On ne lit plus seulement un état (haut ou bas) comme un bouton, on mesure la DURÉE d'une impulsion.
 * C'est cette durée qui donne la distance (le temps que met le son pour faire l'aller-retour).
 * Voir components/ultrasonic pour le fonctionnement du capteur.
 *
 * Tutoriel original : Chapitre 21 - Ultrasonic Ranging (Freenove)
 * À noter que je n'ai fait que le projet 21.1 (le projet 21.2 fait la même chose avec une bibliothèque Arduino).
 */

void start_demo_21_ultrasonic(void);
