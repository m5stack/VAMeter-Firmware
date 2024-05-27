/**
 * @file wifi.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdint.h>
// https://github.com/espressif/esp-idf/blob/v5.1.3/examples/wifi/getting_started/station/main/station_example_main.c

uint8_t wifi_start(const char* ssid, const char* pass);
uint8_t wifi_is_connected();
char* wifi_get_ip();
