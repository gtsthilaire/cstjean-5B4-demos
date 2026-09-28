/**
 * Démonstration de l'utilisation d'un capteur de mouvement MPU6050 (accéléromètre + gyroscope).
 * Chaque seconde, l'accélération (en g) et la vitesse de rotation (en degrés par seconde) sont affichées
 * dans la console.
 *
 * À plat et immobile, on doit lire environ 1 g sur Z (la gravité) et une rotation proche de 0 °/s.
 * Inclinez le capteur : la gravité « passe » sur un autre axe. Tournez-le : la rotation augmente.
 *
 * Voir components/mpu6050 pour le fonctionnement du capteur et la lecture des registres.
 *
 * Tutoriel original : Chapitre 26 - Attitude Sensor MPU6050 (Freenove), projet 26.1
 */

void start_demo_26_mpu6050(void);
