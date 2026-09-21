/**
 * Démonstration de l'utilisation d'un potentiomètre avec l'ADC.
 * La valeur brute lue et la tension approximative sont affichées dans la console.
 *
 * ADC (Analog to Digital Converter) : convertit une tension analogique en valeur numérique.
 * Sur ESP32, l'ADC a une résolution de 12 bits (0 à 4095) pour une plage de 0 à 3.3 V :
 *   Valeur ADC = (Tension / 3.3) * 4095
 *
 * DAC (Digital to Analog Converter) : fait l'inverse, il convertit une valeur numérique en tension.
 * Sur ESP32, les deux DAC ont une résolution de 8 bits (0 à 255) et sont sur les GPIO 25 et 26 :
 *   Tension = (Valeur DAC / 255) * 3.3
 *
 * Tutoriel original : Chapitre 9 - AD/DA Converter (Freenove)
 * À noter que, contrairement au tutoriel, je n'ai pas fait la partie DAC (sortie sur le GPIO 25
 * pour commander la luminosité d'une LED).
 */

void start_demo_09_potentiometer(void);
