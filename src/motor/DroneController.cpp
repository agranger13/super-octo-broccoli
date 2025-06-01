#include "DroneController.h"

DroneController::DroneController(int m1, int m2, int m3, int m4, unsigned int udpPort)
    : motorController(m1, m2, m3, m4),
      udpReceiver(udpPort),
      rollPID(1.0, 0.1, 0.05, 0.01),    // Kp, Ki, Kd, dt
      pitchPID(1.0, 0.1, 0.05, 0.01),
      yawPID(2.0, 0.2, 0.1, 0.01),
      throttle(0), armed(false), emergencyStop(false) {
    
    // Limites de sortie PID
    rollPID.setOutputLimits(-100, 100);
    pitchPID.setOutputLimits(-100, 100);
    yawPID.setOutputLimits(-100, 100);
}

void DroneController::begin(const char* ssid, const char* password) {
    Hotspot hotspot(ssid, password);

    // Connexion WiFi
    if (hotspot.begin()) {
      Serial.print("Hotspot démarré à : ");
      Serial.println(hotspot.getIP());
    } else {
      Serial.println("Échec du démarrage du hotspot");
    }
    
    // Initialisation UDP
    udpReceiver.begin();
    
    // Initialisation de l'IMU
    gyroScoper.begin();
}

void DroneController::updateFromJoystick() {
    // Mise à jour des données UDP
    udpReceiver.update();
    JoystickData joy = udpReceiver.getData();
    
    // Vérifications de sécurité
    if (joy.emergency || !udpReceiver.isConnected()) {
        emergency();
        return;
    }
    
    // Mise à jour de l'état armé
    armed = joy.armed;
    
    if (armed) {
        // Conversion des commandes joystick en consignes de vol
        // Roll: -30° à +30° maximum
        rollSetpoint = joy.leftX * 30.0;
        
        // Pitch: -30° à +30° maximum  
        pitchSetpoint = joy.leftY * 30.0;
        
        // Yaw: vitesse angulaire -180°/s à +180°/s
        yawSetpoint = joy.rightX * 180.0;
        
        // Throttle: 0 à 200 (sur une échelle 0-255)
        throttle = joy.rightY * 200.0;
        
        // Mise à jour du contrôle
        updateControl();
    } else {
        // Drone désarmé - arrêt des moteurs
        motorController.stopAll();
    }
}

void DroneController::setSetpoints(float rollSP, float pitchSP, float yawSP, float throttleSP) {
    rollSetpoint = rollSP;
    pitchSetpoint = pitchSP;
    yawSetpoint = yawSP;
    throttle = constrain(throttleSP, 0, 255);
}

void DroneController::updateControl() {
    // Lecture des données IMU
    gyroScoper.updateGyro();
    
    // Récupération des valeurs
    float rollAngle = gyroScoper.roll;
    float pitchAngle = gyroScoper.pitch;
    float yawRate = gyroScoper.g.gyro.z; // Vitesse angulaire en rad/s
    
    // Calcul des sorties PID
    float rollOutput = rollPID.compute(rollSetpoint, rollAngle);
    float pitchOutput = pitchPID.compute(pitchSetpoint, pitchAngle);
    float yawOutput = yawPID.compute(yawSetpoint, yawRate);
    
    // Configuration moteurs (vue du dessus) :
    //     AVANT
    //   M1   M2
    //     X
    //   M4   M3
    //   ARRIERE
    //
    // M1 et M3 tournent dans le sens horaire
    // M2 et M4 tournent dans le sens anti-horaire
    
    // Calcul des vitesses individuelles des moteurs
    int motor1Speed = throttle - rollOutput - pitchOutput - yawOutput;  // Avant gauche
    int motor2Speed = throttle + rollOutput - pitchOutput + yawOutput;  // Avant droite
    int motor3Speed = throttle + rollOutput + pitchOutput - yawOutput;  // Arrière droite
    int motor4Speed = throttle - rollOutput + pitchOutput + yawOutput;  // Arrière gauche
    
    // Contraindre les valeurs entre 0 et 255
    motor1Speed = constrain(motor1Speed, 0, 255);
    motor2Speed = constrain(motor2Speed, 0, 255);
    motor3Speed = constrain(motor3Speed, 0, 255);
    motor4Speed = constrain(motor4Speed, 0, 255);
    
    // Envoyer les commandes aux moteurs
    motorController.setSpeed(motor1Speed, motor2Speed, motor3Speed, motor4Speed);
}

void DroneController::emergency() {
    motorController.stopAll();
    rollPID.reset();
    pitchPID.reset();
    yawPID.reset();
    armed = false;
    emergencyStop = true;
}

void DroneController::tuneRollPID(float kp, float ki, float kd) {
    rollPID.setTunings(kp, ki, kd);
}

void DroneController::tunePitchPID(float kp, float ki, float kd) {
    pitchPID.setTunings(kp, ki, kd);
}

void DroneController::tuneYawPID(float kp, float ki, float kd) {
    yawPID.setTunings(kp, ki, kd);
}