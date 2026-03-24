// FILE: types.h
#pragma once
#include <stdint.h>

struct CANFrame {
    uint32_t id;
    uint8_t length;
    uint8_t data[8];
    bool extended;
    bool rtr;
    uint32_t timestamp;
};