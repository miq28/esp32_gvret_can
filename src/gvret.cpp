// FILE: gvret.cpp
#include <cstddef>
#include "gvret.h"

void GVRET::processByte(uint8_t b)
{
    // placeholder (next step we plug full parser)
}

bool GVRET::buildFrame(CANFrame& out)
{
    return false;
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