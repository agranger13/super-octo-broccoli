#include "UDPReceiver.h"
#include <cstring>  // Pour strchr
#include <cstdlib>  // Pour abs
#include <Arduino.h> // Pour constrain, millis, Serial

UDPReceiver::UDPReceiver(unsigned int port) : localPort(port) {
    initSafeData();
    currentData = safeData;
    lastPacketTime = 0;
}

void UDPReceiver::begin() {
    udp.begin(localPort);
    Serial.print("UDP Server started on port ");
    Serial.println(localPort);
}

void UDPReceiver::initSafeData() {
    safeData.leftX = 0.0;      // Roll centré
    safeData.leftY = 0.0;      // Pitch centré  
    safeData.rightX = 0.0;     // Yaw centré
    safeData.rightY = 0.0;     // Throttle à zéro
    safeData.armed = false;    // Désarmé
    safeData.emergency = false;
}


bool UDPReceiver::update() {
    int packetSize = udp.parsePacket();
    
    if (packetSize > 0) {
        int length = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
        if (length > 0) {  // Vérifier que la lecture a réussi
            packetBuffer[length] = '\0';
            
            parsePacket(packetBuffer, length);
            lastPacketTime = millis();
            return true;
        }
    }
    
    // Vérification du timeout
    if (millis() - lastPacketTime > TIMEOUT_MS) {
        // Perte de connexion - retour aux valeurs de sécurité
        currentData = safeData;
        return false;
    }
    
    return isConnected();
}

void UDPReceiver::parsePacket(char* buffer, int length) {
    // Format JSON simple: {"lx":512,"ly":480,"rx":500,"ry":100,"arm":1,"emg":0}
    // ou format CSV: 512,480,500,100,1,0
    
    // Mapping des commandes joystick :

    // Left Stick X (leftX) → Roll (-30° à +30°)
    // Left Stick Y (leftY) → Pitch (-30° à +30°)
    // Right Stick X (rightX) → Yaw (-180°/s à +180°/s)
    // Right Stick Y (rightY) → Throttle (0 à 200/255)
    // Armed → Bouton armement (0/1)
    // Emergency → Bouton d'urgence (0/1)

    // Valeurs joystick typiques :

    // Centre : 512 (sur une échelle 0-1023)
    // Zone morte : ±20 autour du centre
    // Plage complète : 0-1023

    // Fonctionnalités de sécurité :

    // Timeout : Arrêt automatique si pas de signal pendant 1 seconde
    // Désarmement automatique : En cas de perte de connexion
    // Arrêt d'urgence : Bouton emergency = 1
    // Zone morte : Évite les micro-mouvements involontaires
    

    // Méthode 1: Format CSV (plus simple)
    if (strchr(buffer, ',') != nullptr) {
        int values[6];
        int count = sscanf(buffer, "%d,%d,%d,%d,%d,%d", 
                          &values[0], &values[1], &values[2], 
                          &values[3], &values[4], &values[5]);
        
        if (count >= 6) {
            currentData.leftX = mapJoystick(values[0]);      // Roll
            currentData.leftY = mapJoystick(values[1]);      // Pitch
            currentData.rightX = mapJoystick(values[2]);     // Yaw
            currentData.rightY = mapJoystick(values[3], 0, 1023, true); // Throttle
            currentData.armed = (values[4] == 1);
            currentData.emergency = (values[5] == 1);
        }
    }
    // Méthode 2: Format JSON simple
    else if (strchr(buffer, '{') != nullptr) {
        int lx, ly, rx, ry, arm, emg;
        int count = sscanf(buffer, "{\"lx\":%d,\"ly\":%d,\"rx\":%d,\"ry\":%d,\"arm\":%d,\"emg\":%d}",
                          &lx, &ly, &rx, &ry, &arm, &emg);
        
        if (count >= 6) {
            currentData.leftX = mapJoystick(lx);
            currentData.leftY = mapJoystick(ly);
            currentData.rightX = mapJoystick(rx);
            currentData.rightY = mapJoystick(ry, 0, 1023, true);
            currentData.armed = (arm == 1);
            currentData.emergency = (emg == 1);
        }
    }
}

float UDPReceiver::mapJoystick(int rawValue, int minVal, int maxVal) {
    // Mapping standard: centre à 512, plage 0-1023 vers -1.0 à +1.0
    float center = (maxVal + minVal) / 2.0f;  // Ajout du 'f' pour float
    float range = (maxVal - minVal) / 2.0f;   // Ajout du 'f' pour float
    
    // Zone morte autour du centre (±20)
    if (abs(rawValue - (int)center) < 20) {  // Cast explicite vers int
        return 0.0f;  // Ajout du 'f' pour float
    }
    
    float normalized = (rawValue - center) / range;
    return constrain(normalized, -1.0f, 1.0f);  // Ajout du 'f' pour float
}

float UDPReceiver::mapJoystick(int rawValue, int minVal, int maxVal, bool throttleMode) {
    if (throttleMode) {
        // Throttle: 0-1023 vers 0.0-1.0
        float normalized = (float)(rawValue - minVal) / (maxVal - minVal);
        return constrain(normalized, 0.0f, 1.0f);  // Ajout du 'f' pour float
    } else {
        return mapJoystick(rawValue, minVal, maxVal);
    }
}

JoystickData UDPReceiver::getData() {
    return currentData;
}

bool UDPReceiver::isConnected() {
    return (millis() - lastPacketTime) < TIMEOUT_MS;
}

void UDPReceiver::printData() {
    Serial.print("Joystick - ");
    Serial.print("Roll: "); Serial.print(currentData.leftX, 2);
    Serial.print(" Pitch: "); Serial.print(currentData.leftY, 2);
    Serial.print(" Yaw: "); Serial.print(currentData.rightX, 2);
    Serial.print(" Throttle: "); Serial.print(currentData.rightY, 2);
    Serial.print(" Armed: "); Serial.print(currentData.armed ? "YES" : "NO");
    Serial.print(" Emergency: "); Serial.print(currentData.emergency ? "YES" : "NO");
    Serial.print(" Connected: "); Serial.println(isConnected() ? "YES" : "NO");
}