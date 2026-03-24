// FILE: tx_ring.h
#pragma once
#include <cstddef>
#include <stdint.h>

template<size_t SIZE>
class ByteRing
{
public:
    bool push(uint8_t b)
    {
        size_t next = (head + 1) % SIZE;
        if (next == tail) return false;

        buffer[head] = b;
        head = next;
        return true;
    }

    size_t pop(uint8_t* out, size_t maxLen)
    {
        size_t count = 0;

        while (count < maxLen && head != tail)
        {
            out[count++] = buffer[tail];
            tail = (tail + 1) % SIZE;
        }

        return count;
    }

    size_t available() const
    {
        return (head >= tail) ? (head - tail) : (SIZE - tail + head);
    }

private:
    volatile size_t head = 0;
    volatile size_t tail = 0;
    uint8_t buffer[SIZE];
};