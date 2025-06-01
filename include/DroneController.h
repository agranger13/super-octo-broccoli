#ifndef DRONECONTROLLER_H
#define DRONECONTROLLER_H

#include "PIDController.h"
#include "MotorController.h"
#include "GyroScoper.h"
#include "UDPReceiver.h"
#include <ESP8266WiFi.h>
#include <Hotspot.h>

class DroneController {
private:
    MotorController motorController;
    GyroScoper gyroScoper;
    UDPReceiver udpReceiver;
    PIDController rollPID;
    PIDController pitchPID;
    PIDController yawPID;
    
    float rollSetpoint;
    float pitchSetpoint;
    float yawSetpoint;
    float throttle;
    
    bool armed;
    bool emergencyStop;

public:
    DroneController(int m1, int m2, int m3, int m4, unsigned int udpPort = 4210);
    
    void begin(const char* ssid, const char* password); // WiFi + IMU
    void updateFromJoystick(); // Lecture UDP et mise à jour contrôle
    void setSetpoints(float rollSP, float pitchSP, float yawSP, float throttleSP);
    void updateControl(); // Plus besoin de paramètres, lit directement l'IMU
    void emergency();
    
    // Méthodes de réglage PID
    void tuneRollPID(float kp, float ki, float kd);
    void tunePitchPID(float kp, float ki, float kd);
    void tuneYawPID(float kp, float ki, float kd);
    
    // Accès aux données IMU pour debug
    float getRoll() { return gyroScoper.roll; }
    float getPitch() { return gyroScoper.pitch; }
    float getYawRate() { return gyroScoper.g.gyro.z; }
    
    // État du drone
    bool isArmed() { return armed; }
    bool isConnected() { return udpReceiver.isConnected(); }
};

#endif