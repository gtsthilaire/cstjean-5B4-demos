## Exemples pour le cours 5B4 - Objets connectés
Ce dépôt regroupe plusieurs démonstrations réalisées en C avec ESP-IDF et basées sur le kit pédagogique Freenove ESP32 (Freenove Ultimate Starter Kit for ESP32).

Ces exemples sont adaptés du tutoriel officiel.

Le projet a été réalisé pour l’enseignement du cours **5B4 – Objets connectés (Cégep de Saint-Jean-sur-Richelieu)**.
Plusieurs éléments sont démontrés en classe et **ce dépôt ne constitue pas un support complet de cours**. Il sert plutôt de référence pour reproduire et adapter les démonstrations vues ensemble.

Ces exemples sont fournis **à titre pédagogique uniquement**. Ils visent à illustrer des concepts de programmation embarquée et d’objets connectés. Ils ne sont **pas validés pour un usage en production** et peuvent contenir des erreurs ou des **simplifications volontairement**. Par exemple, les vérifications d’erreurs et la gestion des retours de fonctions ne sont pas systématiquement implémentées. Ce choix est volontaire afin de garder le code clair et accessible pour l’apprentissage. L’objectif est d’apprendre les bases. Pas de fournir des applications prêtes à l’emploi.


## Structure du projet

```
components/<nom>/        -> logique réutilisable d'un capteur/actionneur (ex: components/led/)
main/demos/NN-<nom>.c/.h -> une démo = un scénario d'usage, numéroté selon les chapitres de Freenove
main/main.c              -> appelle la fonction start_demo_NN_<nom>() de la démo active
```


## Chapitres du tutoriel Freenove

✅ fait · 🟡 fait en partie · ❌ pas fait

Un chapitre est considéré comme fait même s'il manque un composant, lorsque ce composant est traité dans une autre démo.

| Ch. | Sujet (Freenove) | État | Démo(s) | Notes |
|----:|------------------|:----:|---------|-------|
| 0 | LED | ❌ | | |
| 1 | LED | ✅ | `01-led` | |
| 2 | Button & LED | ✅ | `02-push_button` | Sans LED (voir `01-led`) |
| 3 | LED Bar | ✅ | `03-led_bar` | Effet bargraph en plus |
| 4 | Analog & PWM | 🟡 | `04-led_pwm` | Projet 4.1 (Breathing LED) seulement |
| 5 | RGB LED | 🟡 | `05-led_rgb` | Pas de couleurs aléatoires |
| 6 | LEDPixel | ✅ | `06-led_pixel` | |
| 7 | Buzzer | ✅ | `07-buzzer_active`, `07-buzzer_passive` | Sans bouton (voir `02-push_button`) |
| 8 | Serial Communication | ❌ | | |
| 9 | AD/DA Converter | 🟡 | `09-potentiometer` | Partie ADC seulement (pas de DAC) |
| 10 | Touch Sensor | ✅ | `10-touch` | Sans LED (voir `01-led`) |
| 11 | Potentiometer & LED | ❌ | | |
| 12 | Photoresistor & LED | ✅ | `12-photoresistor` | Sans LED (voir `04-led_pwm`) |
| 13 | Thermistor | ✅ | `13-thermistor` | Avec calibration de l'ADC |
| 14 | Joystick | ✅ | `14-joystick` | Position en pourcentage |
| 15 | 74HC595 & LED Bar Graph | ❌ | | |
| 16 | 74HC595 & 7-Segment Display | ❌ | | |
| 16 | 74HC595 & LED Matrix | ✅ | `16-led_matrix` | |
| 17 | Relay & Motor | ❌ | | |
| 17 | Motor & Driver | ❌ | | |
| 18 | Servo | ❌ | | En révision |
| 19 | Stepper Motor | ❌ | | |
| 20 | LCD1602 | ✅ | `20-lcd1602` | |
| 21 | Ultrasonic Ranging | ❌ | | |
| 22 | Matrix Keypad | ❌ | | |
| 23 | Infrared Remote | ❌ | | |
| 24 | Hygrothermograph DHT11 | ❌ | | |
| 25 | Infrared Motion Sensor | ❌ | | |
| 26 | Attitude Sensor MPU6050 | ❌ | | |
| 27 | Bluetooth | ❌ | | |
| 28 | Bluetooth Media by DAC | ❌ | | |
| 29 | Bluetooth Media by Audio Module | ❌ | | |
| 30 | Read and Write the Sdcard | ❌ | | |
| 31 | Play SD card music | ❌ | | |
| 32 | WiFi Working Modes | ✅ | `32-wifi_station`, `32-wifi_ap` | Projets 32.1 (Station) et 32.2 (AP) |
| 33 | TCP/IP | ❌ | | En révision |
| 34 | Camera Web Server | ❌ | | En révision |
| 35 | Camera Tcp Server | ❌ | | |
| 36 | Soldering Circuit Board | ❌ | | |


## Configuration (Wi-Fi)

Les démos réseau lisent leurs paramètres (SSID, mot de passe, clés d'API) dans le menu de configuration,
section **Configuration des exemples**. Cette section est définie dans [main/Kconfig.projbuild](main/Kconfig.projbuild) :
c'est là qu'on ajoute un nouveau paramètre, qu'on lit ensuite dans le code avec le préfixe `CONFIG_`
(ex : `CONFIG_EXEMPLES_WIFI_SSID`).

On utilise le menuconfig classique, dans un terminal (l'éditeur graphique de l'extension VS Code n'affiche pas
cette section).

1. Ouvrir un terminal ESP-IDF. Dans Visual Studio Code : palette de commandes (`Ctrl+Shift+P`, ou `Cmd+Shift+P`
   sur macOS) → **ESP-IDF: Open ESP-IDF Terminal**.
2. Lancer :

   ```
   idf.py menuconfig
   ```

3. Avec les flèches, descendre jusqu'à **Configuration des exemples**, puis `Entrée`.
4. Choisir un paramètre, `Entrée` pour le modifier, puis `Entrée` pour valider.
5. `S` pour enregistrer, puis `Q` pour quitter.
6. Recompiler et flasher le projet pour que les nouvelles valeurs soient prises en compte.

Les valeurs sont enregistrées dans `sdkconfig`, qui n'est pas suivi par git : les secrets ne sont donc jamais commités.


## Références utiles

[Tutoriel Freenove](https://docs.freenove.com/projects/fnk0047/en/latest/fnk0047/codes/C.html)

[Documentation ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)


## Licence

Ce projet inclut des exemples adaptés du tutoriel de Freenove.  
Le contenu original de Freenove est sous licence [Creative Commons Attribution-NonCommercial-ShareAlike 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/).  

En conséquence, ce dépôt est également publié sous la même licence :  

**CC BY-NC-SA 3.0**  
- Attribution : vous devez créditer l’auteur original (Freenove) et l’adaptateur (ce projet).  
- NonCommercial : vous ne pouvez pas utiliser ce contenu à des fins commerciales.  
- ShareAlike : si vous modifiez ou réutilisez ce contenu, vous devez le redistribuer sous la même licence.  

Voir le fichier [LICENSE](LICENSE.txt) pour les détails complets.