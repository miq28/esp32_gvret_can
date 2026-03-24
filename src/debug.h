// FILE: debug.h
#pragma once
#include "rs485.h"

#define LOGI(fmt, ...) \
    do { RS485.printf("[INFO] " fmt "\r\n", ##__VA_ARGS__); } while (0)

#define LOGE(fmt, ...) \
    do { RS485.printf("[ERR] " fmt "\r\n", ##__VA_ARGS__); } while (0)
