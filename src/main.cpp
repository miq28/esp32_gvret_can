// FILE: main.cpp
#include <Arduino.h>
#include "system_manager.h"
#include "rs485.h"
#include "led_manager.h"
#include "debug.h"

SystemManager sys;

void setup()
{
    RS485.begin(1000000);
    Serial.begin(1000000);

    ledInit(50);

    ledSet(255, 0, 0); // RED
    delay(500);
    ledSet(0, 255, 0); // GREEN
    delay(500);
    ledSet(0, 0, 255); // BLUE
    delay(500);

    LOGI("BOOT OK");

    sys.setup();
}

void loop()
{
    sys.loop();
    ledTask();
}