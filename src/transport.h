// FILE: transport/transport.h
#pragma once
#include <stddef.h>
#include <stdint.h>

class Transport
{
public:
    virtual size_t write(const uint8_t* data, size_t len) = 0;
    virtual int read() = 0;
    virtual bool available() = 0;
};