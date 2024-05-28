/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdint.h>
// https://github.com/espressif/esp-idf/blob/v5.1.3/examples/system/ota/simple_ota_example/main/simple_ota_example.c

typedef void (*SimpleOtaOnLog_t)(char*);

uint8_t simple_ota_start_via_http(const char* firmwareUrl, SimpleOtaOnLog_t onLog);
