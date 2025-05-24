#include "UdpListener.h"
#include <Arduino.h>

UdpListener::UdpListener(uint16_t port) : port(port) {}

bool UdpListener::begin() {
    return udp.begin(port);
}

void UdpListener::listen() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        int len = udp.read(buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';
            Serial.print("Reçu : ");
            Serial.println(buffer);
            Serial.print("De : ");
            Serial.println(udp.remoteIP());
        }
    }
}
