// FILE: gvret.cpp
#include <Arduino.h>
#include "gvret.h"
#include <cstring>

void GVRET::processByte(uint8_t b)
{
    switch (state)
    {
    case WAIT_START:
        if (b == 0xF1)
        {
            state = READ_CMD;
        }
        break;

    case READ_CMD:
        cmd = b;
        state = READ_LEN;
        break;

    case READ_LEN:
        len = b;
        idx = 0;
        state = READ_DATA;
        break;

    case READ_DATA:
        buffer[idx++] = b;
        if (idx >= len)
        {
            frameReady = true;
            state = WAIT_START;
        }
        break;
    }
}

bool GVRET::buildFrame(CANFrame& out)
{
    if (!frameReady) return false;
    frameReady = false;

    if (cmd != 0) return false; // only CAN frame

    uint32_t id =
        buffer[0] |
        (buffer[1] << 8) |
        (buffer[2] << 16) |
        (buffer[3] << 24);

    out.extended = (id & (1UL << 31));
    out.id = id & 0x1FFFFFFF;

    out.length = buffer[4] & 0x0F;

    for (int i = 0; i < out.length; i++)
        out.data[i] = buffer[5 + i];

    out.rtr = false;
    out.timestamp = micros();

    return true;
}

size_t GVRET::encodeFrame(const CANFrame& f, uint8_t* out)
{
    size_t idx = 0;

    out[idx++] = 0xF1;
    out[idx++] = 0;

    uint32_t t = f.timestamp;
    out[idx++] = t;
    out[idx++] = t >> 8;
    out[idx++] = t >> 16;
    out[idx++] = t >> 24;

    uint32_t id = f.id | (f.extended ? (1UL << 31) : 0);

    out[idx++] = id;
    out[idx++] = id >> 8;
    out[idx++] = id >> 16;
    out[idx++] = id >> 24;

    out[idx++] = f.length;

    for (int i = 0; i < f.length; i++)
        out[idx++] = f.data[i];

    out[idx++] = 0;

    return idx;
}