#include "MotorController.h"

MotorController::MotorController(int m1, int m2, int m3, int m4) 
  : pinM1(m1), pinM2(m2), pinM3(m3), pinM4(m4) {
  for (int pin : {pinM1, pinM2, pinM3, pinM4}) {
    pinMode(pin, OUTPUT);
  }
}

void MotorController::setSpeed(int m1, int m2, int m3, int m4) {
  analogWrite(pinM1, constrain(m1, 0, 255));
  analogWrite(pinM2, constrain(m2, 0, 255));
  analogWrite(pinM3, constrain(m3, 0, 255));
  analogWrite(pinM4, constrain(m4, 0, 255));
}

void MotorController::stopAll() {
  setSpeed(0, 0, 0, 0);
}
