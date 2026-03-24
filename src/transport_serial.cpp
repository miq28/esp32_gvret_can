// FILE: transport/transport_serial.cpp
#include "transport.h"
#include <Arduino.h>

class SerialTransport : public Transport
{
public:
    size_t write(const uint8_t* d, size_t l) override { return Serial.write(d, l); }
    int read() override { return Serial.read(); }
    bool available() override { return Serial.available(); }
};