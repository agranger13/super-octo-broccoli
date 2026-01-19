#ifndef DRONECONTROLLER_H
#define DRONECONTROLLER_H

#include "PIDController.h"
#include "MotorController.h"
#include "GyroScoper.h"
#include "UDPReceiver.h"
#include <Hotspot.h>

class DroneController {
private:
    MotorController motorController;  // Contrôleur des moteurs
    GyroScoper gyroScoper;            // Capteur gyroscopique
    
    PIDController rollPID;           // Contrôleur PID pour le roll
    PIDController pitchPID;          // Contrôleur PID pour le pitch
    PIDController yawPID;            // Contrôleur PID pour le yaw
    
    float rollSetpoint;              // Consigne de roll (degrés)
    float pitchSetpoint;             // Consigne de pitch (degrés)
    float yawSetpoint;               // Consigne de yaw (degrés/s)
    float throttle;                  // Consigne de throttle (0-255)
    
    bool armed;                      // État armé/désarmé du drone
    bool emergencyStop;              // Indicateur d'arrêt d'urgence

public:
    /**
     * Constructeur du contrôleur de drone
     * @param m1 Pin du moteur 1 (avant gauche)
     * @param m2 Pin du moteur 2 (avant droite)
     * @param m3 Pin du moteur 3 (arrière droite)
     * @param m4 Pin du moteur 4 (arrière gauche)
     * @param udpPort Port UDP pour la réception des commandes (par défaut: 4210)
     */
    DroneController(int m1, int m2, int m3, int m4, unsigned int udpPort = 4210);
    
    UDPReceiver udpReceiver;         // Récepteur UDP pour les commandes
    
    /**
     * Initialise le drone (WiFi, UDP, IMU)
     * @param ssid SSID du réseau WiFi
     * @param password Mot de passe du réseau WiFi
     */
    void begin(const char* ssid, const char* password);
    
    /**
     * Met à jour le contrôle du drone à partir des données du joystick
     * Lit les données UDP et applique les commandes
     */
    void updateFromJoystick();
    
    /**
     * Définit les consignes de vol
     * @param rollSP Consigne de roll (degrés)
     * @param pitchSP Consigne de pitch (degrés)
     * @param yawSP Consigne de yaw (degrés/s)
     * @param throttleSP Consigne de throttle (0-255)
     */
    void setSetpoints(float rollSP, float pitchSP, float yawSP, float throttleSP);
    
    /**
     * Met à jour le contrôle du drone en utilisant les données de l'IMU
     * Calcule les sorties PID et envoie les commandes aux moteurs
     */
    void updateControl();
    
    /**
     * Arrêt d'urgence du drone
     * Arrête les moteurs et réinitialise les contrôleurs PID
     */
    void emergency();
    
    // Méthodes de réglage PID
    void tuneRollPID(float kp, float ki, float kd);
    void tunePitchPID(float kp, float ki, float kd);
    void tuneYawPID(float kp, float ki, float kd);
    
    // Accès aux données IMU pour debug
    float getRoll() { return gyroScoper.roll; }
    float getPitch() { return gyroScoper.pitch; }
    float getYawRate() { return gyroScoper.g.gyro.z; }
    
    // Accès aux consignes
    float getRollSetpoint() { return rollSetpoint; }
    float getPitchSetpoint() { return pitchSetpoint; }
    float getYawSetpoint() { return yawSetpoint; }
    float getThrottle() { return throttle; }
    
    // État du drone
    bool isArmed() { return armed; }
    bool isConnected() { return udpReceiver.isConnected(); }
    bool isEmergency() { return emergencyStop; }
    
    // Accès au récepteur UDP
    UDPReceiver& getUDPReceiver() { return udpReceiver; }

};

#endif