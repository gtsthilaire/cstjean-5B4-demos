/**
 * Démonstration de l'utilisation d'une LED avec PWM.
 * La luminosité de la LED augmente progressivement jusqu'au maximum, puis diminue jusqu'à l'extinction.
 *
 * Pour nos GPIO, une sortie numérique est soit haute (3.3V) soit basse (0V).
 *
 * Le PWM (Pulse Width Modulation) est une technique qui permet de simuler une sortie analogique en faisant varier
 * la largeur d'une impulsion numérique. En variant le rapport cyclique (duty cycle) de l'impulsion, on peut contrôler
 * la puissance moyenne délivrée à une charge, comme une LED.
 *
 * Sur une période de temps fixe, une impulsion PWM alterne entre un état haut (3.3V) et un état bas (0V). Le rapport cyclique
 * est défini comme le pourcentage de temps pendant lequel l'impulsion est haute par rapport à la période totale. Par exemple,
 * un rapport cyclique de 50% signifie que l'impulsion est haute pendant la moitié du temps et basse pendant l'autre moitié.
 * On aura donc une puissance moyenne de 50% de la puissance maximale.
 *
 * Le PWM est souvent utilisé pour :
 * - Le contrôle de la luminosité des LED
 * - Le contrôle de la vitesse des moteurs
 * - La génération de signaux audio
 * - Etc.
 *
 * Tutoriel original : Chapitre 4 - Analog and PWM (Freenove)
 * À noter que je n'ai fait que le projet 4.1 (Breathing LED).
 */

void start_demo_04_led_pwm(void);
