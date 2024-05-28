/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <stdint.h>
// https://github.com/espressif/esp-idf/blob/v5.1.3/examples/wifi/getting_started/station/main/station_example_main.c

uint8_t wifi_start(const char* ssid, const char* pass);
uint8_t wifi_is_connected();
char* wifi_get_ip();
