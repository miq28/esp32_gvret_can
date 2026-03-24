// FILE: rs485.h
#pragma once
#include <Arduino.h>

class RS485Class : public HardwareSerial
{
public:
    RS485Class() : HardwareSerial(1) {}

    void begin(uint32_t baud)
    {
        HardwareSerial::begin(baud, SERIAL_8N1, 21, 22);
        pinMode(17, OUTPUT);
        digitalWrite(17, HIGH);
    }
};

extern RS485Class RS485;