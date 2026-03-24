// FILE: gvret.cpp
#include <Arduino.h>
#include "gvret.h"
#include <cstring>

void GVRET::processByte(uint8_t b)
{
    // enable binary mode
    if (b == 0xE7 && state == WAIT_START)
    {
        binaryMode = true;
        return;
    }

    switch (state)
    {
    case WAIT_START:
        if (b == 0xF1)
            state = GET_COMMAND;
        break;

    case GET_COMMAND:
        cmd = b;
        commandReady = true;

        if (cmd == 0x00)
        {
            buildIdx = 0;
            state = BUILD_FRAME;
            commandReady = false;
        }
        else
        {
            state = WAIT_START;
        }
        break;

    case BUILD_FRAME:
        buildBuf[buildIdx++] = b;

        // need at least ID(4) + bus(1) + len(1)
        if (buildIdx >= 6)
        {
            uint8_t len = buildBuf[5] & 0x0F;

            if (buildIdx >= (6 + len))
            {
                // frame complete
                state = WAIT_START;
            }
        }
        break;
    }
}

bool GVRET::handleCommand(uint8_t *outBuf, size_t &outLen)
{
    if (!commandReady)
        return false;

    commandReady = false;

    outLen = 0;

    switch (cmd)
    {
    // ===== KEEPALIVE =====
    case 0x09:
        outBuf[outLen++] = 0xF1;
        outBuf[outLen++] = 0x09;
        outBuf[outLen++] = 0xDE;
        outBuf[outLen++] = 0xAD;
        return true;

    // ===== DEVICE INFO =====
    case 0x07:
        outBuf[outLen++] = 0xF1;
        outBuf[outLen++] = 0x07;

        outBuf[outLen++] = 0x01; // build LSB
        outBuf[outLen++] = 0x00; // build MSB
        outBuf[outLen++] = 0x01; // EEPROM
        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        return true;

    // ===== GET CAN CONFIG =====
    case 0x06:
        outBuf[outLen++] = 0xF1;
        outBuf[outLen++] = 0x06;

        outBuf[outLen++] = 0x01; // enabled
        outBuf[outLen++] = 0x20;
        outBuf[outLen++] = 0xA1;
        outBuf[outLen++] = 0x07;
        outBuf[outLen++] = 0x00;

        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        outBuf[outLen++] = 0x00;
        return true;

    // ===== TIME SYNC =====
    case 0x01:
    {
        uint32_t t = micros();

        outBuf[outLen++] = 0xF1;
        outBuf[outLen++] = 0x01;

        outBuf[outLen++] = t;
        outBuf[outLen++] = t >> 8;
        outBuf[outLen++] = t >> 16;
        outBuf[outLen++] = t >> 24;
        return true;
    }

    default:
        return false;
    }
}

size_t GVRET::encodeFrame(const CANFrame &f, uint8_t *out)
{
    size_t idx = 0;

    out[idx++] = 0xF1;
    out[idx++] = 0x00;

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

    out[idx++] = (0 << 4) | (f.length & 0x0F);

    for (int i = 0; i < f.length; i++)
        out[idx++] = f.data[i];

    out[idx++] = 0;

    return idx;
}

bool GVRET::buildFrame(CANFrame &frame)
{
    // only valid after BUILD_FRAME state filled buffer
    if (cmd != 0x00)
        return false;

    // minimal safety
    if (buildIdx < 6)
        return false;

    // ===== parse ID =====
    uint32_t id = 0;
    id |= buildBuf[0];
    id |= (buildBuf[1] << 8);
    id |= (buildBuf[2] << 16);
    id |= (buildBuf[3] << 24);

    frame.extended = (id & (1UL << 31)) != 0;
    frame.id = id & 0x1FFFFFFF;

    // ===== bus (ignored for now) =====
    uint8_t bus = buildBuf[4];
    (void)bus;

    // ===== length =====
    frame.length = buildBuf[5] & 0x0F;

    if (frame.length > 8)
        frame.length = 8;

    // ===== data =====
    for (uint8_t i = 0; i < frame.length; i++)
    {
        frame.data[i] = buildBuf[6 + i];
    }

    frame.timestamp = micros();
    frame.rtr = false;

    // reset buffer for next frame
    buildIdx = 0;

    return true;
}
