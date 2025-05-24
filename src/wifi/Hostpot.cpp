#include "Hotspot.h"

Hotspot::Hotspot(const char* ssid, const char* password,
                         IPAddress ip, IPAddress gateway, IPAddress subnet)
    : ssid(ssid), password(password), ip(ip), gateway(gateway), subnet(subnet) {}

bool Hotspot::begin() {
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, gateway, subnet);
    return WiFi.softAP(ssid, password);
}

IPAddress Hotspot::getIP() {
    return WiFi.softAPIP();
}
