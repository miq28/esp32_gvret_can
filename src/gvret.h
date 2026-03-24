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

private:
    enum State {
        WAIT_START,
        READ_CMD,
        READ_LEN,
        READ_DATA
    };

    State state = WAIT_START;

    uint8_t cmd;
    uint8_t len;
    uint8_t idx;

    uint8_t buffer[32];
    bool frameReady = false;
};