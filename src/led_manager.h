// FILE: led_manager.h
#pragma once
#include <stdint.h>

void ledInit(uint8_t brightness);
void ledSet(uint8_t r, uint8_t g, uint8_t b);

void ledCanActivity();
void ledWifiConnected(bool state);
void ledTask();