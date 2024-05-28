/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include "stdint.h"
// https://github.com/espressif/esp-idf/tree/v5.1.3/examples/peripherals/usb/device/tusb_msc

uint8_t msc_mode_init();
uint8_t msc_mode_deinit();
