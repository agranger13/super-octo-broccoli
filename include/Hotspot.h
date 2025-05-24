#ifndef WIFI_HOTSPOT_H
#define WIFI_HOTSPOT_H

#include <ESP8266WiFi.h>

class Hotspot {
public:
    Hotspot(const char* ssid, const char* password,
                IPAddress ip = IPAddress(192,168,4,1),
                IPAddress gateway = IPAddress(192,168,4,1),
                IPAddress subnet = IPAddress(255,255,255,0));

    bool begin();
    IPAddress getIP();

private:
    const char* ssid;
    const char* password;
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;
};

#endif