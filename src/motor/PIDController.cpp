#include "PIDController.h"

PIDController::PIDController(float kp, float ki, float kd, float dt)
    : kp(kp), ki(ki), kd(kd), dt(dt), integral(0), previousError(0),
      outputMin(-1000), outputMax(1000) {}

void PIDController::setTunings(float kp, float ki, float kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

void PIDController::setOutputLimits(float min, float max) {
    outputMin = min;
    outputMax = max;
}

void PIDController::reset() {
    integral = 0;
    previousError = 0;
}

float PIDController::compute(float setpoint, float input) {
    float error = setpoint - input;
    integral += error * dt;
    float derivative = (error - previousError) / dt;
    previousError = error;

    float output = kp * error + ki * integral + kd * derivative;

    // Saturation (anti-windup partiel)
    if (output > outputMax) output = outputMax;
    else if (output < outputMin) output = outputMin;

    return output;
}
