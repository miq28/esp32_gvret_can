// FILE: can_ring.h
#pragma once
#include <cstddef>
#include "types.h"

template<size_t SIZE>
class CANRing
{
public:
    bool push(const CANFrame& f)
    {
        size_t next = (head + 1) % SIZE;
        if (next == tail) return false;
        buffer[head] = f;
        head = next;
        return true;
    }

    bool pop(CANFrame& f)
    {
        if (head == tail) return false;
        f = buffer[tail];
        tail = (tail + 1) % SIZE;
        return true;
    }

private:
    volatile size_t head = 0;
    volatile size_t tail = 0;
    CANFrame buffer[SIZE];
};