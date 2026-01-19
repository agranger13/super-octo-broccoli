#include <Arduino.h>
#include <UdpListener.h>
#include <PIDController.h>
#include <GyroScoper.h>
#include <DroneController.h>
#include <DroneLogger.h>

// Configuration WiFi
const char* ssid = "DRONE_IOT";
const char* password = "dronepassword";

// Configuration des broches des moteurs
// Schéma de câblage:
//     AVANT
//   M1   M2
//     X
//   M4   M3
//   ARRIERE
#define M1 D2 // Moteur 1 - Avant Gauche (sens horaire)
#define M2 D3 // Moteur 2 - Avant Droite (sens anti-horaire)
#define M3 D6 // Moteur 3 - Arrière Droite (sens horaire)
#define M4 D5 // Moteur 4 - Arrière Gauche (sens anti-horaire)
DroneController drone(M1, M2, M3, M4);
DroneLogger logger(drone);

void setup() {
  // Initialisation de la communication série
  Serial.begin(9600);
  while (!Serial)
    delay(10);

  // Initialiser le drone (WiFi + UDP + IMU)
  // Cette méthode configure le hotspot WiFi, le serveur UDP et l'IMU
  drone.begin(ssid, password);

  // Initialiser le logger
  logger.begin();

  // Réglages PID initiaux (à ajuster selon vos tests)
  // Format: tune[Axis]PID(Kp, Ki, Kd)
  // Kp: Proportional gain - Réponse immédiate à l'erreur
  // Ki: Integral gain - Correction de l'erreur accumulée
  // Kd: Derivative gain - Amortissement des oscillations
  drone.tuneRollPID(1.2, 0.15, 0.08);   // Roll PID
  drone.tunePitchPID(1.2, 0.15, 0.08);  // Pitch PID
  drone.tuneYawPID(2.5, 0.3, 0.1);      // Yaw PID
}


void loop()
{
  // Mise à jour automatique depuis les commandes UDP
  // Cette méthode lit les données UDP, met à jour les consignes de vol
  // et envoie les commandes aux moteurs si le drone est armé
  drone.updateFromJoystick();
  
  // Mise à jour du logger (refresh toutes les 10 secondes)
  logger.update();

  delay(10); // Petite pause pour éviter une boucle trop rapide
}
