#ifndef DRONE_LOGGER_H
#define DRONE_LOGGER_H

#include <Arduino.h>
#include <DroneController.h>

class DroneLogger {
public:
    DroneLogger(DroneController& droneRef) : drone(droneRef) {}
    
    void begin() {
        lastLogTime = millis();
        logEnabled = true;
    }
    
    void update() {
        if (!logEnabled) return;
        
        unsigned long currentTime = millis();
        if (currentTime - lastLogTime >= 10000) { // 10 secondes
            logDroneStatus();
            lastLogTime = currentTime;
        }
    }
    
    void enableLogging(bool enable) {
        logEnabled = enable;
        if (enable) {
            Serial.println("=== Drone Logging ENABLED ===");
        } else {
            Serial.println("=== Drone Logging DISABLED ===");
        }
    }
    
    void logCritical(const String& message) {
        Serial.print("[CRITICAL] ");
        Serial.println(message);
    }
    
    void logWarning(const String& message) {
        Serial.print("[WARNING] ");
        Serial.println(message);
    }
    
    void logInfo(const String& message) {
        Serial.print("[INFO] ");
        Serial.println(message);
    }
    
private:
    DroneController& drone;
    unsigned long lastLogTime;
    bool logEnabled = true;
    
    void logDroneStatus() {
        Serial.println("\n=== DRONE STATUS REPORT ===");
        
        // État général
        Serial.print("Armed: ");
        Serial.print(drone.isArmed() ? "YES" : "NO");
        Serial.print(" | Connected: ");
        Serial.print(drone.isConnected() ? "YES" : "NO");
        Serial.print(" | Emergency: ");
        Serial.println(drone.isEmergency() ? "YES" : "NO");
        
        // Données de vol
        Serial.print("Roll: ");
        Serial.print(drone.getRoll(), 2);
        Serial.print("° | Pitch: ");
        Serial.print(drone.getPitch(), 2);
        Serial.print("° | YawRate: ");
        Serial.print(drone.getYawRate(), 2);
        Serial.println("°/s");
        
        // Consignes
        Serial.print("Roll SP: ");
        Serial.print(drone.getRollSetpoint(), 2);
        Serial.print("° | Pitch SP: ");
        Serial.print(drone.getPitchSetpoint(), 2);
        Serial.print("° | Yaw SP: ");
        Serial.print(drone.getYawSetpoint(), 2);
        Serial.println("°/s");
        
        // Throttle
        Serial.print("Throttle: ");
        Serial.print(drone.getThrottle());
        Serial.println("/255");
        
        // Données UDP
        Serial.println("=== JOYSTICK DATA ===");
        drone.getUDPReceiver().printData();
        
        Serial.println("=== END STATUS REPORT ===\n");
    }
};

#endif // DRONE_LOGGER_H