// FILE: can_driver.h
#pragma once
#include "types.h"

class CANDriver
{
public:
    bool begin(uint32_t baud, bool listenOnly);
    bool send(const CANFrame& f);
    bool receive(CANFrame& f);
};