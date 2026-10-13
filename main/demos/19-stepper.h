/**
 * Démonstration de l'utilisation d'un moteur pas-à-pas avec un ULN2003.
 * Le moteur fait un tour en 4 quarts de tour (512 pas chacun), avec une pause à chaque quart :
 * il s'arrête chaque fois à une position précise.
 *
 * Voir components/stepper pour le fonctionnement du moteur pas-à-pas (et pourquoi un tour = 2048 pas).
 *
 * Attention : le moteur doit être alimenté par une source externe de 5V.
 *
 * Tutoriel original : Chapitre 19 - Stepper Motor (Freenove)
 */

void start_demo_19_stepper(void);
