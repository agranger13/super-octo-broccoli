#include "MotorController.h"

MotorController::MotorController(int m1, int m2, int m3, int m4)
  : pinM1(m1), pinM2(m2), pinM3(m3), pinM4(m4) {
  // Aucune configuration matérielle ici : voir begin()
}

bool MotorController::begin() {
  // Attachement des broches au PWM (canal LEDC alloué automatiquement)
  bool ok = true;
  ok &= ledcAttach(pinM1, freq, resolution);
  ok &= ledcAttach(pinM2, freq, resolution);
  ok &= ledcAttach(pinM3, freq, resolution);
  ok &= ledcAttach(pinM4, freq, resolution);

  // Sécurité : moteurs à l'arrêt dès l'initialisation
  stopAll();

  if (!ok) {
    Serial.println("ERREUR: attachement PWM des moteurs impossible!");
  }
  return ok;
}

void MotorController::setSpeed(int m1, int m2, int m3, int m4) {
  ledcWrite(pinM1, constrain(m1, 0, 255));
  ledcWrite(pinM2, constrain(m2, 0, 255));
  ledcWrite(pinM3, constrain(m3, 0, 255));
  ledcWrite(pinM4, constrain(m4, 0, 255));
}

void MotorController::stopAll() {
  setSpeed(0, 0, 0, 0);
}
