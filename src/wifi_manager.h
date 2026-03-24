// FILE: wifi_manager.h
#pragma once
#include <WiFi.h>

class WiFiManager
{
public:
    void setup(const char* ssid, const char* pass, bool apMode);
    void loop();

    bool connected();
    size_t send(const uint8_t* data, size_t len);
    int read();
    bool available();

private:
    WiFiServer server{23};
    WiFiClient client;
    bool hasClient = false;
};