#include <Arduino.h>
#include <UdpListener.h>
#include <PIDController.h>
#include <GyroScoper.h>
#include <DroneController.h>

// Configuration WiFi
const char* ssid = "DRONE_IOT";
const char* password = "dronepassword";

#define M1 D10 // Avant Gauche
#define M2 D11 // Avant Droite
#define M4 D12 // Arriere Gauche
#define M3 D13 // Arriere Droite
DroneController drone(M1, M2, M3, M4);

void setup() {
  Serial.begin(9600);
  while (!Serial)
    delay(10);

  // Initialiser le drone (WiFi + UDP + IMU)
  drone.begin(ssid, password);

  // Réglages PID initiaux (à ajuster selon vos tests)
  drone.tuneRollPID(1.2, 0.15, 0.08);
  drone.tunePitchPID(1.2, 0.15, 0.08);
  drone.tuneYawPID(2.5, 0.3, 0.1);
}


void loop()
{
  // Mise à jour automatique depuis les commandes UDP
  drone.updateFromJoystick();
  
  // Debug (optionnel)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 100) {
      lastPrint = millis();
      Serial.print("Roll: "); Serial.print(drone.getRoll(), 2);
      Serial.print(" Pitch: "); Serial.print(drone.getPitch(), 2);
      Serial.print(" YawRate: "); Serial.println(drone.getYawRate(), 2);
  }
}
