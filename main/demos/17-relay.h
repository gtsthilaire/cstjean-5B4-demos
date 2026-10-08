/**
 * Démonstration de l'utilisation d'un relais pour commander un moteur.
 * Le moteur tourne pendant 2 secondes, puis s'arrête pendant 2 secondes, en boucle.
 *
 * Un moteur DC (courant continu) tourne dès qu'on l'alimente. Mais il consomme trop de courant pour être branché
 * directement sur un GPIO : c'est le relais qui fait l'interrupteur entre le moteur et son alimentation.
 * Voir components/relay pour le fonctionnement du relais.
 *
 * Le code est le même que pour la démo 01 (une LED) : seul le montage change.
 * La démo 17-dc_motor montre comment choisir en plus la vitesse et le sens de rotation.
 *
 * Attention : le moteur demande trop de courant pour l'USB seul. Au démarrage du moteur, la tension chute et l'ESP32
 * redémarre (ou la liaison USB se coupe). Il faut brancher une pile 9V sur l'entrée 7-9V de la carte d'extension
 * (l'USB reste branché pour les logs). Sans pile, on peut faire la démo sans le moteur : on entend le relais claquer.
 *
 * Tutoriel original : Chapitre 17 - Relay & Motor (Freenove)
 * À noter que, contrairement au tutoriel, il n'y a pas de bouton ni de LED.
 */

void start_demo_17_relay(void);
