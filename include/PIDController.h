#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PIDController {
public:
    PIDController(float kp, float ki, float kd, float dt);
    
    void setTunings(float kp, float ki, float kd);
    void setOutputLimits(float min, float max);
    void reset();

    float compute(float setpoint, float input);

private:
    float kp, ki, kd, dt;
    float integral;
    float previousError;
    float outputMin, outputMax;
};

#endif
