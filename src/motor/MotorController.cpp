#include "MotorController.h"

MotorController::MotorController(int m1, int m2, int m3, int m4) 
  : pinM1(m1), pinM2(m2), pinM3(m3), pinM4(m4) {
  
  // Configuration des canaux PWM pour ESP32
  ledcSetup(channelM1, freq, resolution);
  ledcSetup(channelM2, freq, resolution);
  ledcSetup(channelM3, freq, resolution);
  ledcSetup(channelM4, freq, resolution);
  
  // Attachement des pins aux canaux
  ledcAttachPin(pinM1, channelM1);
  ledcAttachPin(pinM2, channelM2);
  ledcAttachPin(pinM3, channelM3);
  ledcAttachPin(pinM4, channelM4);
}

void MotorController::setSpeed(int m1, int m2, int m3, int m4) {
  ledcWrite(channelM1, constrain(m1, 0, 255));
  ledcWrite(channelM2, constrain(m2, 0, 255));
  ledcWrite(channelM3, constrain(m3, 0, 255));
  ledcWrite(channelM4, constrain(m4, 0, 255));
}

void MotorController::stopAll() {
  setSpeed(0, 0, 0, 0);
}