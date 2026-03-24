// FILE: gvret.h
#pragma once
#include <cstddef>
#include "types.h"

class GVRET
{
public:
    void processByte(uint8_t b);
    bool buildFrame(CANFrame& out);
    size_t encodeFrame(const CANFrame& f, uint8_t* out);
};