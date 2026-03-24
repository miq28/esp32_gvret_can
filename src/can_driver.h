// FILE: can_driver.h
#pragma once

#include <Arduino.h>
#include "types.h"
#include <driver/twai.h>

// ===== callback type =====
typedef void (*TwaiEventCallback)(const char *msg);

class CANDriver
{
public:
    bool begin(uint32_t baud, bool listenOnly);
    bool send(const CANFrame &f);
    bool receive(CANFrame &f);

    void setEventCallback(TwaiEventCallback cb);
    void debugStatus();

private:
    TwaiEventCallback eventCb = nullptr;
};