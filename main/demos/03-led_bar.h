/**
 * Démonstration de l'utilisation d'une barre de LED.
 * Deux effets s'enchaînent en boucle :
 *  1. Lumière qui voyage : une seule LED est allumée à la fois et elle se déplace d'un bout à l'autre de la barre,
 *     aller-retour.
 *  2. Bargraph : la barre se remplit, puis se vide (led_bar_set_level). C'est la base d'un affichage de niveau,
 *     par exemple pour montrer la position d'un potentiomètre ou un niveau sonore.
 *
 * Une barre de LED n'est rien d'autre que 10 LED indépendantes regroupées dans un même boîtier.
 * Chaque LED est reliée à son propre GPIO (avec sa résistance) et se contrôle donc exactement
 * comme la LED simple de la démo 01.
 *
 * Tutoriel original : Chapitre 3 - LED Bar (Freenove)
 * À noter que, contrairement au tutoriel, j'ai ajouté l'effet bargraph.
 */

void start_demo_03_led_bar(void);
