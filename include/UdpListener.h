#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <WiFiUdp.h>

class UdpListener {
public:
    UdpListener(uint16_t port = 1234);
    bool begin();
    void listen();

private:
    WiFiUDP udp;
    uint16_t port;
    char buffer[255];
};

#endif
