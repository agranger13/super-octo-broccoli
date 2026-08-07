#include "DroneController.h"

DroneController::DroneController(int m1, int m2, int m3, int m4, unsigned int udpPort)
    : motorController(m1, m2, m3, m4),
      gyroScoper(),
      udpReceiver(udpPort),
      rollPID(1.0, 0.1, 0.05, 0.01),
      pitchPID(1.0, 0.1, 0.05, 0.01),
      yawPID(2.0, 0.2, 0.1, 0.01),
      rollSetpoint(0), pitchSetpoint(0), yawSetpoint(0),
      throttle(0), armed(true), emergencyStop(false) {

    // Limites de sortie PID
    // Ces limites empêchent les corrections trop importantes
    // qui pourraient endommager les moteurs ou rendre le drone instable
    rollPID.setOutputLimits(-100, 100);
    pitchPID.setOutputLimits(-100, 100);
    yawPID.setOutputLimits(-100, 100);
}

void DroneController::begin(const char* ssid, const char* password) {
    // Initialisation des sorties PWM en premier : les moteurs sont mis à
    // l'arrêt avant toute autre initialisation
    motorController.begin();

    Hotspot hotspot(ssid, password);

    // Connexion WiFi
    hotspot.begin();

    // Initialisation UDP
    udpReceiver.begin();

    // Initialisation de l'IMU
    gyroScoper.begin();
}

void DroneController::updateFromJoystick() {
    // Mise à jour des données UDP
    udpReceiver.update();
    JoystickData joy = udpReceiver.getData();

    // Trace de diagnostic : signale uniquement les changements d'état,
    // pour ne pas saturer le port série à chaque tour de boucle
    static bool prevArmed = false;
    static bool prevConnected = false;
    bool nowConnected = udpReceiver.isConnected();
    if (nowConnected != prevConnected) {
        Serial.print("[LIEN] "); Serial.println(nowConnected ? "CONNECTE" : "PERDU (timeout)");
        prevConnected = nowConnected;
    }
    if (joy.armed != prevArmed) {
        Serial.print("[ARM] joystick demande: "); Serial.println(joy.armed ? "ARME" : "DESARME");
        prevArmed = joy.armed;
    }

    // Vérifications de sécurité
    if (joy.emergency || !nowConnected) {
        emergency();
        return;
    }

    // Sortie du verrou d'urgence : uniquement si le lien est rétabli, que
    // l'urgence est relâchée ET que le joystick est revenu au neutre désarmé.
    // On impose ce passage par "désarmé" pour interdire un redémarrage
    // spontané des moteurs avec du gaz déjà appliqué.
    if (emergencyStop) {
        if (!joy.armed) {
            emergencyStop = false;
            Serial.println("[ARM] verrou d'urgence relache - pret a armer");
        } else {
            // Toujours verrouillé : moteurs à l'arrêt tant que l'opérateur
            // n'a pas remis l'armement à zéro
            motorController.stopAll();
            return;
        }
    }

    // Mise à jour de l'état armé
    armed = joy.armed;

    if (armed) {
        // Conversion des commandes joystick en consignes de vol
        rollSetpoint = joy.leftX * MAX_ROLL_ANGLE;
        pitchSetpoint = joy.leftY * MAX_PITCH_ANGLE;
        yawSetpoint = joy.rightX * MAX_YAW_RATE;
        throttle = joy.rightY * MAX_THROTTLE;

        // Mise à jour du contrôle
        updateControl();
    } else {
        // Drone désarmé - arrêt des moteurs
        motorController.stopAll();
    }
}

void DroneController::setSetpoints(float rollSP, float pitchSP, float yawSP, float throttleSP) {
    // Définit les consignes de vol
    // Les consignes sont utilisées par la méthode updateControl() pour calculer les sorties PID
    rollSetpoint = rollSP;
    pitchSetpoint = pitchSP;
    yawSetpoint = yawSP;
    // Contraindre le throttle entre 0 et 255 pour éviter les valeurs hors plage
    throttle = constrain(throttleSP, 0, 255);
}

void DroneController::updateControl() {
    // Met à jour le contrôle du drone en utilisant les données de l'IMU
    // Cette méthode calcule les sorties PID et envoie les commandes aux moteurs
    // Elle est appelée par updateFromJoystick() lorsque le drone est armé

    // Pas de correction PID sous le seuil de vol (évite l'accumulation intégrale au sol)
    if (throttle < 20) {
        rollPID.reset();
        pitchPID.reset();
        yawPID.reset();
        motorController.setSpeed(throttle, throttle, throttle, throttle);
        return;
    }

    // Lecture des données IMU
    // Cette méthode met à jour les angles de roll, pitch et la vitesse angulaire
    // Elle doit être appelée régulièrement pour obtenir des données précises
    gyroScoper.updateGyro();

    // Récupération des valeurs de l'IMU
    // Ces valeurs sont mises à jour par la méthode updateGyro() de l'IMU
    // Elles représentent l'état actuel du drone
    float rollAngle = gyroScoper.roll;      // Angle de roll en degrés
    float pitchAngle = gyroScoper.pitch;     // Angle de pitch en degrés
    float yawRate = gyroScoper.g.gyro.z * 180.0f / PI;  // Conversion rad/s → deg/s

    // Calcul des sorties PID
    // Les contrôleurs PID calculent la correction nécessaire pour atteindre la consigne
    // Format: compute(consigne, valeur_actuelle)
    // Retourne une valeur entre -100 et +100 (limites définies dans le constructeur)
    float rollOutput = rollPID.compute(rollSetpoint, rollAngle);
    float pitchOutput = pitchPID.compute(pitchSetpoint, pitchAngle);
    float yawOutput = yawPID.compute(yawSetpoint, yawRate);

    // Configuration moteurs (vue du dessus):
    //     AVANT
    //   M1   M2
    //     X
    //   M4   M3
    //   ARRIERE
    //
    // M1 et M3 tournent dans le sens horaire
    // M2 et M4 tournent dans le sens anti-horaire
    // Cette configuration permet un contrôle stable du drone

    // Calcul des vitesses individuelles des moteurs
    // Formules basées sur la configuration en X des moteurs:
    // - Roll: M1 et M4 diminuent, M2 et M3 augmentent
    // - Pitch: M1 et M2 diminuent, M3 et M4 augmentent
    // - Yaw: M1 et M3 diminuent, M2 et M4 augmentent
    // Le throttle est ajouté à chaque moteur pour maintenir l'altitude
    int motor1Speed = throttle - rollOutput - pitchOutput - yawOutput;  // Avant gauche
    int motor2Speed = throttle + rollOutput - pitchOutput + yawOutput;  // Avant droite
    int motor3Speed = throttle + rollOutput + pitchOutput - yawOutput;  // Arrière droite
    int motor4Speed = throttle - rollOutput + pitchOutput + yawOutput;  // Arrière gauche

    // Envoyer les commandes aux moteurs (contrainte appliquée dans MotorController)
    // Les vitesses des moteurs sont envoyées au contrôleur des moteurs
    // Le contrôleur des moteurs se charge de générer les signaux PWM appropriés
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
    // Règle les paramètres du contrôleur PID pour le roll
    // Kp: Proportional gain - Réponse immédiate à l'erreur
    // Ki: Integral gain - Correction de l'erreur accumulée
    // Kd: Derivative gain - Amortissement des oscillations
    rollPID.setTunings(kp, ki, kd);
}

void DroneController::tunePitchPID(float kp, float ki, float kd) {
    // Règle les paramètres du contrôleur PID pour le pitch
    pitchPID.setTunings(kp, ki, kd);
}

void DroneController::tuneYawPID(float kp, float ki, float kd) {
    // Règle les paramètres du contrôleur PID pour le yaw
    yawPID.setTunings(kp, ki, kd);
}