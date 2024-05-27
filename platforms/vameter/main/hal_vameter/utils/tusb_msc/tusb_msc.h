/**
 * @file tusb_msc.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "stdint.h"
// https://github.com/espressif/esp-idf/tree/v5.1.3/examples/peripherals/usb/device/tusb_msc

uint8_t msc_mode_init();
uint8_t msc_mode_deinit();
