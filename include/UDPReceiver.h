#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <WiFiUdp.h>
#include <Arduino.h>

// Constantes
#define TIMEOUT_MS 10000          // Timeout de connexion en millisecondes (10 secondes)
#define PACKET_BUFFER_SIZE 256   // Taille du buffer de réception (256 octets)
#define JOYSTICK_MIN 0           // Valeur minimale du joystick (0)
#define JOYSTICK_MAX 1023        // Valeur maximale du joystick (1023)
#define JOYSTICK_CENTER 512      // Valeur centrale du joystick (512)
#define DEADZONE 20              // Zone morte du joystick (±20 autour du centre)

// Structure pour les données du joystick
struct JoystickData {
    float leftX;        // Roll (-1.0 à +1.0) - Commande de roulis
    float leftY;        // Pitch (-1.0 à +1.0) - Commande de tangage
    float rightX;       // Yaw (-1.0 à +1.0) - Commande de lacet
    float rightY;       // Throttle (0.0 à 1.0) - Commande de gaz
    bool armed;         // État armé/désarmé du drone
    bool emergency;     // Arrêt d'urgence
};

class UDPReceiver {
private:
    WiFiUDP udp;                                    // Instance UDP pour la communication
    unsigned int localPort;                         // Port d'écoute UDP
    char packetBuffer[PACKET_BUFFER_SIZE];         // Buffer de réception des données
    unsigned long lastPacketTime;                   // Timestamp du dernier paquet reçu (millis)
    
    JoystickData currentData;                       // Données actuelles du joystick
    JoystickData safeData;                         // Données de sécurité (utilisées en cas de timeout)
    
    /**
     * Initialise les valeurs de sécurité pour le joystick
     * Ces valeurs sont utilisées en cas de perte de connexion
     */
    void initSafeData();
    
    /**
     * Parse les données reçues depuis le buffer UDP
     * @param buffer Buffer contenant les données reçues
     * @param length Longueur des données dans le buffer
     */
    void parsePacket(char* buffer, int length);
    
    /**
     * Convertit une valeur brute du joystick en une valeur normalisée
     * @param rawValue Valeur brute du joystick (0-1023)
     * @param minVal Valeur minimale du joystick (par défaut: JOYSTICK_MIN)
     * @param maxVal Valeur maximale du joystick (par défaut: JOYSTICK_MAX)
     * @return Valeur normalisée (-1.0 à +1.0)
     */
    float mapJoystick(int rawValue, int minVal = JOYSTICK_MIN, int maxVal = JOYSTICK_MAX);
    
    /**
     * Convertit une valeur brute du joystick en une valeur normalisée
     * @param rawValue Valeur brute du joystick (0-1023)
     * @param minVal Valeur minimale du joystick
     * @param maxVal Valeur maximale du joystick
     * @param throttleMode Mode throttle (0.0 à 1.0) ou mode standard (-1.0 à +1.0)
     * @return Valeur normalisée
     */
    float mapJoystick(int rawValue, int minVal, int maxVal, bool throttleMode);

public:
    // Constructeur
    UDPReceiver(unsigned int port = 4210);
    
    /**
     * Démarre le serveur UDP pour recevoir les commandes
     */
    void begin();
    
    /**
     * Met à jour les données UDP (à appeler dans loop())
     * @return true si des données ont été reçues, false sinon
     */
    bool update();
    
    /**
     * Retourne les données actuelles du joystick
     * @return Structure JoystickData contenant les commandes
     */
    JoystickData getData();
    
    /**
     * Vérifie si la connexion UDP est active
     * @return true si la connexion est active, false sinon
     */
    bool isConnected();
    
    /**
     * Affiche les données du joystick sur le port série
     * Format: Roll | Pitch | Yaw | Throttle | Armed | Emergency | Connected
     */
    void printData();
    
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