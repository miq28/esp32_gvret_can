// FILE: led_manager.cpp
#include "led_manager.h"
#include <Arduino.h>

#define LED_PIN 4

static uint32_t lastCanBlink = 0;
static bool wifiConnected = false;

void ledInit(uint8_t brightness)
{
    pinMode(LED_PIN, OUTPUT);
}

void ledSet(uint8_t r, uint8_t g, uint8_t b)
{
    // VERY SIMPLE (no neopixel lib yet)
    digitalWrite(LED_PIN, (r | g | b) ? HIGH : LOW);
}

void ledCanActivity()
{
    lastCanBlink = millis();
}

void ledWifiConnected(bool state)
{
    wifiConnected = state;
}

void ledTask()
{
    if (millis() - lastCanBlink < 50)
    {
        ledSet(0, 50, 0); // green pulse
    }
    else if (wifiConnected)
    {
        ledSet(0, 0, 50); // blue
    }
    else
    {
        ledSet(50, 0, 0); // red
    }
}