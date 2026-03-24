// FILE: can_driver.cpp
#include <cstring>
#include <Arduino.h>
#include "can_driver.h"
#include "driver/twai.h"
#include "board_pins.h"

bool CANDriver::begin(uint32_t baud, bool listenOnly)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)BoardPins::CAN_TX,
        (gpio_num_t)BoardPins::CAN_RX,
        listenOnly ? TWAI_MODE_LISTEN_ONLY : TWAI_MODE_NORMAL
    );

    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    if (baud == 1000000) t_config = TWAI_TIMING_CONFIG_1MBITS();

    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
        return false;

    if (twai_start() != ESP_OK)
        return false;

    return true;
}

bool CANDriver::send(const CANFrame& f)
{
    twai_message_t m = {};
    m.identifier = f.id;
    m.extd = f.extended;
    m.rtr = f.rtr;
    m.data_length_code = f.length;
    memcpy(m.data, f.data, f.length);

    return twai_transmit(&m, 0) == ESP_OK;
}

bool CANDriver::receive(CANFrame& f)
{
    twai_message_t m;
    if (twai_receive(&m, 0) != ESP_OK) return false;

    f.id = m.identifier;
    f.extended = m.extd;
    f.rtr = m.rtr;
    f.length = m.data_length_code;
    f.timestamp = micros();

    memcpy(f.data, m.data, f.length);
    return true;
}