/**
 * Démonstration de l'utilisation d'un module WS2812 (LED pixel / NeoPixel).
 * Les LED s'allument une à une en rouge, puis en vert, bleu, jaune et cyan. Ensuite, un arc-en-ciel tourne
 * pendant 5 secondes (couleurs définies par leur teinte, en HSV), puis tout s'éteint.
 * La luminosité globale est réduite (LED_PIXEL_BRIGHTNESS) pour ne pas être éblouissante.
 *
 * Voir components/led_pixel pour le fonctionnement des WS2812 et la dépendance au composant led_strip.
 *
 * Tutoriel original : Chapitre 6 - LEDPixel (Freenove)
 */

void start_demo_06_led_pixel(void);
