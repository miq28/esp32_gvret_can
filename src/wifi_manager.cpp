// FILE: wifi_manager.cpp
#include "wifi_manager.h"
#include "led_manager.h"
#include "debug.h"

void WiFiManager::setup(const char* ssid, const char* pass, bool apMode)
{
    WiFi.mode(apMode ? WIFI_AP : WIFI_STA);
    WiFi.setSleep(false);

    if (apMode)
    {
        WiFi.softAP(ssid, pass);
        LOGI("AP: %s", ssid);
    }
    else
    {
        WiFi.begin(ssid, pass);
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
        }
        ledWifiConnected(true);
        LOGI("STA IP: %s", WiFi.localIP().toString().c_str());
    }

    server.begin();
}

void WiFiManager::loop()
{
    if (server.hasClient())
    {
        if (client)
            client.stop();

        client = server.available();
        hasClient = true;
        LOGI("WiFi client connected");
    }

    if (client && !client.connected())
    {
        client.stop();
        hasClient = false;
        LOGI("WiFi client disconnected");
    }
}

bool WiFiManager::connected()
{
    return hasClient;
}

size_t WiFiManager::send(const uint8_t* data, size_t len)
{
    if (hasClient)
        return client.write(data, len);

    return Serial.write(data, len);
}

int WiFiManager::read()
{
    if (hasClient && client.available())
        return client.read();

    return Serial.read();
}

bool WiFiManager::available()
{
    if (hasClient)
        return client.available();

    return Serial.available();
}