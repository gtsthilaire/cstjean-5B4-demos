/**
 * Démonstration de l'utilisation d'une LED RGB.
 * La LED affiche successivement les couleurs primaires (rouge, vert, bleu), leurs mélanges deux à deux
 * (jaune, cyan, magenta) et le blanc (les trois mélangées), puis fait un fondu sur la roue des couleurs.
 *
 * Une LED RGB contient trois LED (rouge, verte, bleue). En réglant l'intensité de chacune avec du PWM,
 * on peut obtenir n'importe quelle couleur (mélange additif). Voir la démo 04 pour une explication du PWM.
 *
 * Tutoriel original : Chapitre 5 - RGB LED (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de couleurs aléatoires.
 */

void start_demo_05_led_rgb(void);
