/**
 * Démonstration de l'utilisation d'un buzzer passif.
 * Le buzzer imite une sirène : sa fréquence monte et descend en suivant une sinusoïde (2000 Hz ± 500 Hz).
 *
 * Contrairement au buzzer actif, le buzzer passif a besoin d'un signal PWM : c'est la fréquence du signal qui
 * détermine la note. Voir la démo 04 pour une explication du PWM.
 *
 * Tutoriel original : Chapitre 7 - Buzzer (Freenove), projet 7.2 (Alertor)
 * À noter que, contrairement au tutoriel, je n'ai pas fait de gestion de bouton pour activer/désactiver le buzzer.
 */

void start_demo_07_buzzer_passive(void);
