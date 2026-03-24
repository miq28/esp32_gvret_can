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
    ledSet(0, 0, 0); // RED

    LOGI("BOOT OK");

    sys.setup();
}

void loop()
{
    sys.loop();

    ledTask();
}