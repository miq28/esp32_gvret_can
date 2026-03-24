// FILE: system_manager.cpp
#include "system_manager.h"
#include <Arduino.h>
#include "wifi_manager.h"
#include "debug.h"

ByteRing<8192> txRing;

static WiFiManager wifi;
static uint8_t txBuf[64];

// ===== callback =====
static void logTwai(const char *msg)
{
    LOGI("%s", msg); // routed to RS485 via debug.h
}

void SystemManager::setup()
{
    driver.begin(500000, false);

    driver.setEventCallback(logTwai);

    // wifi.setup("esp32ret_XXXXXX", "12345678", true); // AP mode
    wifi.setup("galaxi", "n1n4iqb4l", false); // STA mode
}

void SystemManager::loop()
{
    CANFrame f;

    wifi.loop();

    // CAN RX → ring
    // int drainBudget = 64; // tune later
    // while (drainBudget--){}
    while (true)
    {
        if (!driver.receive(f))
            break;

        // LOGI("CAN RX id=%X len=%d", f.id, f.length);

        if (!rxRing.push(f))
        {
            LOGE("RX overflow");
        }
    }

    // ring → GVRET → transport
    while (rxRing.pop(f))
    {
        size_t len = gvret.encodeFrame(f, txBuf);
        wifi.send(txBuf, len);
    }

    // ENCODE → TX RING (NOT DIRECT SEND)
    uint8_t tempBuf[64];

    int encodeBudget = 64;

    while (encodeBudget-- && rxRing.pop(f))
    {
        size_t len = gvret.encodeFrame(f, tempBuf);

        for (size_t i = 0; i < len; i++)
        {
            if (!txRing.push(tempBuf[i]))
            {
                LOGE("TX overflow");
                break;
            }
        }
    }

    // SEND (LIMITED, NON-BLOCKING STYLE)
    uint8_t sendBuf[256];

    size_t toSend = txRing.pop(sendBuf, sizeof(sendBuf));

    if (toSend > 0)
    {
        if (wifi.connected())
        {
            wifi.send(sendBuf, toSend);
        }
        else
        {
            Serial.write(sendBuf, toSend);
        }
    }

    // RX from host
    // int rxBudget = 32;

    // while (rxBudget-- && wifi.available())
    // {
    //     int b = wifi.read();
    //     if (b < 0)
    //         break;

    //     gvret.processByte((uint8_t)b);

    //     CANFrame out;
    //     if (gvret.buildFrame(out))
    //     {
    //         LOGI("CAN TX id=%X len=%d", out.id, out.length);
    //         driver.send(out);
    //     }
    // }

    while (Serial.available())
    {
        uint8_t b = Serial.read();

        gvret.processByte(b);

        // ===== handle commands immediately =====
        uint8_t resp[64];
        size_t respLen = 0;

        if (gvret.handleCommand(resp, respLen))
        {
            Serial.write(resp, respLen); // 🔴 DO NOT BUFFER
        }

        // ===== CAN TX from host =====
        CANFrame out;
        if (gvret.buildFrame(out))
        {
            LOGI("CAN TX id=%X len=%d", out.id, out.length);
            driver.send(out);
        }
    }

    // ===== DEBUG STATUS (lightweight, safe every loop) =====
    driver.debugStatus();
}