/**
 * Démonstration de l'utilisation d'une matrice de LED 8x8 avec deux 74HC595.
 * La matrice affiche un visage souriant, puis les lettres A et B, 3 secondes chacun.
 *
 * Deux façons d'écrire un motif sont montrées :
 *  - le visage est écrit avec des octets (un par colonne, un bit par rangée), comme dans le tutoriel ;
 *  - les lettres sont dessinées en texte, ligne par ligne ('#' = LED allumée), ce qui est plus lisible.
 *
 * Voir components/led_matrix pour le fonctionnement du 74HC595 et du balayage des colonnes.
 *
 * Tutoriel original : Chapitre 16 - 74HC595 & LED Matrix (Freenove)
 */

void start_demo_16_led_matrix(void);
