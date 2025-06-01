#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <WiFiUdp.h>
#include <Arduino.h>

// Constantes
#define TIMEOUT_MS 10000          // Timeout de connexion en millisecondes
#define PACKET_BUFFER_SIZE 256   // Taille du buffer de réception
#define JOYSTICK_MIN 0           // Valeur minimale du joystick
#define JOYSTICK_MAX 1023        // Valeur maximale du joystick
#define JOYSTICK_CENTER 512      // Valeur centrale du joystick
#define DEADZONE 20              // Zone morte du joystick

// Structure pour les données du joystick
struct JoystickData {
    float leftX;        // Roll (-1.0 à +1.0)
    float leftY;        // Pitch (-1.0 à +1.0)
    float rightX;       // Yaw (-1.0 à +1.0)
    float rightY;       // Throttle (0.0 à 1.0)
    bool armed;         // État armé/désarmé
    bool emergency;     // Arrêt d'urgence
};

class UDPReceiver {
private:
    WiFiUDP udp;                                    // Instance UDP
    unsigned int localPort;                         // Port d'écoute
    char packetBuffer[PACKET_BUFFER_SIZE];         // Buffer de réception
    unsigned long lastPacketTime;                   // Timestamp du dernier paquet reçu
    
    JoystickData currentData;                       // Données actuelles
    JoystickData safeData;                         // Données de sécurité
    
    // Méthodes privées
    void initSafeData();                           // Initialise les valeurs de sécurité
    void parsePacket(char* buffer, int length);    // Parse les données reçues
    float mapJoystick(int rawValue, int minVal = JOYSTICK_MIN, int maxVal = JOYSTICK_MAX);
    float mapJoystick(int rawValue, int minVal, int maxVal, bool throttleMode);

public:
    // Constructeur
    UDPReceiver(unsigned int port = 4210);
    
    // Méthodes publiques
    void begin();                                  // Démarre le serveur UDP
    bool update();                                 // Met à jour les données (à appeler dans loop())
    JoystickData getData();                        // Retourne les données actuelles
    bool isConnected();                            // Vérifie si la connexion est active
    void printData();                              // Affiche les données sur le port série
    
    // Getters individuels (optionnels)
    float getRoll() { return currentData.leftX; }
    float getPitch() { return currentData.leftY; }
    float getYaw() { return currentData.rightX; }
    float getThrottle() { return currentData.rightY; }
    bool isArmed() { return currentData.armed; }
    bool isEmergency() { return currentData.emergency; }
    
    // Setters pour les paramètres (optionnels)
    void setTimeout(unsigned long timeoutMs) { /* Could implement dynamic timeout */ }
    unsigned int getPort() { return localPort; }
};

#endif // UDPRECEIVER_H