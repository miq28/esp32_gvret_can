// FILE: led_manager.cpp
#include <Arduino.h>
#include <driver/rmt.h>
#include "led_manager.h"

#define LED_PIN 4
#define RMT_TX_CHANNEL RMT_CHANNEL_0

static uint8_t ledBrightness = 255;

static uint32_t canActivityCount = 0;
static uint32_t lastUpdate = 0;

static uint32_t ledColor = 0;
static uint32_t lastCanBlink = 0;
static bool wifiConnected = false;

static void sendBit(bool bit, rmt_item32_t *item)
{
    if (bit)
    {
        // T1H ~0.8us, T1L ~0.45us
        item->level0 = 1;
        item->duration0 = 14;
        item->level1 = 0;
        item->duration1 = 6;
    }
    else
    {
        // T0H ~0.4us, T0L ~0.85us
        item->level0 = 1;
        item->duration0 = 6;
        item->level1 = 0;
        item->duration1 = 14;
    }
}

void ledInit(uint8_t brightness)
{
    ledBrightness = brightness;

    rmt_config_t config = {};
    config.channel = RMT_TX_CHANNEL;
    config.gpio_num = (gpio_num_t)LED_PIN;
    config.clk_div = 4;
    config.mem_block_num = 1;
    config.rmt_mode = RMT_MODE_TX;

    rmt_config(&config);
    rmt_driver_install(config.channel, 0, 0);
}

void ledSet(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t r2 = (r * ledBrightness) / 255;
    uint8_t g2 = (g * ledBrightness) / 255;
    uint8_t b2 = (b * ledBrightness) / 255;

    ledColor = (g2 << 16) | (r2 << 8) | b2;

    rmt_item32_t items[24];

    for (int i = 0; i < 24; i++)
    {
        bool bit = ledColor & (1 << (23 - i));
        sendBit(bit, &items[i]);
    }

    rmt_write_items(RMT_TX_CHANNEL, items, 24, true);
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);

    // reset pulse
    rmt_item32_t resetItem = {};
    resetItem.level0 = 0;
    resetItem.duration0 = 100;
    resetItem.level1 = 0;
    resetItem.duration1 = 0;

    rmt_write_items(RMT_TX_CHANNEL, &resetItem, 1, true);
    rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);
}

void ledCanActivity()
{
    canActivityCount++;
}

void ledWifiConnected(bool state)
{
    wifiConnected = state;
}

void ledTask()
{
    uint32_t now = millis();

    if (now - lastUpdate > 50) // update every 50ms
    {
        lastUpdate = now;

        if (canActivityCount > 0)
        {
            ledSet(0, 50, 0); // green pulse
            canActivityCount = 0;
        }
        else if (wifiConnected)
        {
            ledSet(0, 0, 50); // blue
        }
        else
        {
            ledSet(50, 0, 0); // red
        }
    }
}