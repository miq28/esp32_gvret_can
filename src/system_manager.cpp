// FILE: system_manager.cpp
#include "system_manager.h"
#include <Arduino.h>
#include "wifi_manager.h"
#include "debug.h"

static WiFiManager wifi;
static uint8_t txBuf[64];

void SystemManager::setup()
{
    driver.begin(500000, false);

    // wifi.setup("esp32ret_XXXXXX", "12345678", true); // AP mode
    wifi.setup("galaxi", "n1n4iqb4l", false); // STA mode
}

void SystemManager::loop()
{
    CANFrame f;

    wifi.loop();

    // CAN RX → ring
    while (driver.receive(f))
    {
        LOGI("CAN RX id=%X len=%d", f.id, f.length);
        if (!rxRing.push(f))
            LOGE("RX overflow");
    }

    // ring → GVRET → transport
    while (rxRing.pop(f))
    {
        size_t len = gvret.encodeFrame(f, txBuf);
        wifi.send(txBuf, len);
    }

    // RX from host
    while (wifi.available())
    {
        int b = wifi.read();
        if (b < 0)
            break;

        gvret.processByte((uint8_t)b);

        CANFrame out;
        if (gvret.buildFrame(out))
        {
            driver.send(out);
        }
    }
}