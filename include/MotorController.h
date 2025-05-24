#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

class MotorController {
  private:
    int pinM1, pinM2, pinM3, pinM4;

  public:
    MotorController(int m1, int m2, int m3, int m4);
    void setSpeed(int m1, int m2, int m3, int m4);
    void stopAll();
};

#endif // MOTOR_CONTROLLER_H