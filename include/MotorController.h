#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

class MotorController {
private:
  int pinM1, pinM2, pinM3, pinM4;
  
  // Canaux PWM (ESP32 a 16 canaux: 0-15)
  const int channelM1 = 0;
  const int channelM2 = 1;
  const int channelM3 = 2;
  const int channelM4 = 3;
  
  // Configuration PWM
  const int freq = 20000;      // Fréquence 5kHz (bon pour moteurs DC)
  const int resolution = 8;    // Résolution 8-bit (0-255)

public:
  MotorController(int m1, int m2, int m3, int m4);
  void setSpeed(int m1, int m2, int m3, int m4);
  void stopAll();
};

#endif