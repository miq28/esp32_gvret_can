// FILE: system_manager.cpp
#include "system_manager.h"
#include <Arduino.h>

static uint8_t txBuf[64];

void SystemManager::setup()
{
    driver.begin(500000, false);
}

void SystemManager::loop()
{
    CANFrame f;

    // CAN → ring
    while (driver.receive(f))
    {
        rxRing.push(f);
    }

    // ring → GVRET → Serial
    while (rxRing.pop(f))
    {
        size_t len = gvret.encodeFrame(f, txBuf);
        Serial.write(txBuf, len);
    }

    // Serial → GVRET → CAN
    while (Serial.available())
    {
        uint8_t b = Serial.read();
        gvret.processByte(b);

        CANFrame out;
        if (gvret.buildFrame(out))
        {
            driver.send(out);
        }
    }
}