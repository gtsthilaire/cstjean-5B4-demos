/**
 * Démonstration de l'utilisation de l'écran LCD1602 (via son module I2C).
 * Trois affichages se répètent en boucle :
 *  1. Texte statique sur les deux lignes (avec une valeur aléatoire).
 *  2. Texte plus long que l'écran qui défile (décalage matériel de l'affichage).
 *  3. Texte qui défile sur la première ligne pendant que la deuxième reste fixe.
 *
 * Tutoriel original : Chapitre 20 - LCD1602 (Freenove)
 */

void start_demo_20_lcd1602(void);
