// FILE: gvret.h
#pragma once
#include <Arduino.h>
#include "types.h"

class GVRET
{
public:
    void processByte(uint8_t b);

    bool handleCommand(uint8_t* outBuf, size_t& outLen);
    bool buildFrame(CANFrame& frame);

    size_t encodeFrame(const CANFrame& f, uint8_t* out);

private:
    enum State
    {
        WAIT_START,
        GET_COMMAND,
        BUILD_FRAME
    };

    State state = WAIT_START;

    bool binaryMode = false;
    uint8_t cmd = 0;

    uint8_t buildBuf[32];
    uint8_t buildIdx = 0;

    bool commandReady = false;   // ✅ CORRECT PLACE
};