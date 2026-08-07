#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

class MotorController {
private:
  int pinM1, pinM2, pinM3, pinM4;

  // Configuration PWM
  // Depuis le core Arduino-ESP32 3.x, les canaux LEDC sont alloués
  // automatiquement : on pilote directement par numéro de broche.
  const int freq = 20000;      // Fréquence 20kHz (au-dessus de l'audible)
  const int resolution = 8;    // Résolution 8-bit (0-255)

public:
  MotorController(int m1, int m2, int m3, int m4);

  // Configure les sorties PWM. À appeler depuis setup(), jamais depuis un
  // constructeur global : le périphérique LEDC n'est pas prêt avant.
  bool begin();

  void setSpeed(int m1, int m2, int m3, int m4);
  void stopAll();
};

#endif
