/**
 * Démonstration de l'utilisation d'un moteur DC avec un L293D.
 * Le moteur accélère jusqu'à sa vitesse maximale, ralentit jusqu'à l'arrêt, puis fait la même chose dans l'autre sens.
 *
 * C'est le même moteur que la démo 17-relay, mais le L293D permet en plus de choisir :
 *   - le sens de rotation (2 GPIO) ;
 *   - la vitesse (PWM, comme la luminosité de la LED de la démo 04).
 * Voir components/dc_motor pour le fonctionnement du L293D.
 *
 * Attention : comme pour la démo 17-relay, le moteur demande trop de courant pour l'USB seul. Il faut brancher
 * une pile 9V sur l'entrée 7-9V de la carte d'extension (l'USB reste branché pour les logs).
 *
 * Tutoriel original : Chapitre 17 - Motor & Driver (Freenove)
 * À noter que, contrairement au tutoriel, il n'y a pas de potentiomètre (voir la démo 09).
 */

void start_demo_17_dc_motor(void);
