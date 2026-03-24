// FILE: can_driver.cpp
#include "can_driver.h"
#include <cstring>
#include "led_manager.h"

bool CANDriver::begin(uint32_t baud, bool listenOnly)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
        (gpio_num_t)27, // TX
        (gpio_num_t)26, // RX
        listenOnly ? TWAI_MODE_LISTEN_ONLY : TWAI_MODE_NORMAL);

    // 🔴 important tuning
    g_config.rx_queue_len = 512;
    g_config.tx_queue_len = 32;

    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK)
        return false;

    if (twai_start() != ESP_OK)
        return false;

    return true;
}

bool CANDriver::send(const CANFrame &f)
{
    twai_message_t m = {};
    m.identifier = f.id;
    m.extd = f.extended;
    m.rtr = f.rtr;
    m.data_length_code = f.length;
    memcpy(m.data, f.data, f.length);

    return twai_transmit(&m, 0) == ESP_OK;
}

bool CANDriver::receive(CANFrame &f)
{
    twai_message_t m;

    // NON-BLOCKING
    if (twai_receive(&m, 0) != ESP_OK)
        return false;

    ledCanActivity();

    f.id = m.identifier;
    f.extended = m.extd;
    f.rtr = m.rtr;
    f.length = m.data_length_code;
    f.timestamp = micros();

    memcpy(f.data, m.data, f.length);

    return true;
}

void CANDriver::setEventCallback(TwaiEventCallback cb)
{
    eventCb = cb;
}

void CANDriver::debugStatus()
{
    static bool initialized = false;

    static uint32_t last_missed = 0;
    static uint32_t last_overrun = 0;
    static uint32_t last_bus_err = 0;
    static uint32_t last_state = 0;

    twai_status_info_t status;
    twai_get_status_info(&status);

    // skip first sample (startup noise)
    if (!initialized)
    {
        last_missed = status.rx_missed_count;
        last_overrun = status.rx_overrun_count;
        last_bus_err = status.bus_error_count;
        last_state = status.state;
        initialized = true;
        return;
    }

    // ===== STATE CHANGE =====
    if (status.state != last_state)
    {
        char buf[64];
        sprintf(buf, "[TWAI] STATE CHANGE: %d -> %d", last_state, status.state);
        if (eventCb)
            eventCb(buf);

        // warn if not running
        if (status.state != 1)
        {
            char warn[64];
            sprintf(warn, "[TWAI] WARNING: state=%d", status.state);
            if (eventCb)
                eventCb(warn);
        }

        last_state = status.state;
    }

    // ===== RX MISSED =====
    if (status.rx_missed_count != last_missed)
    {
        char buf[64];
        sprintf(buf, "[TWAI] RX MISSED +%d (total=%d)",
                status.rx_missed_count - last_missed,
                status.rx_missed_count);

        if (eventCb)
            eventCb(buf);
        last_missed = status.rx_missed_count;
    }

    // ===== RX OVERRUN =====
    if (status.rx_overrun_count != last_overrun)
    {
        char buf[64];
        sprintf(buf, "[TWAI] RX OVERRUN +%d (total=%d)",
                status.rx_overrun_count - last_overrun,
                status.rx_overrun_count);

        if (eventCb)
            eventCb(buf);
        last_overrun = status.rx_overrun_count;
    }

    // ===== BUS ERROR =====
    if (status.bus_error_count != last_bus_err)
    {
        char buf[64];
        sprintf(buf, "[TWAI] BUS ERROR +%d (total=%d)",
                status.bus_error_count - last_bus_err,
                status.bus_error_count);

        if (eventCb)
            eventCb(buf);
        last_bus_err = status.bus_error_count;
    }
}