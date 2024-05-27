/**
 * @file simple_ota.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdint.h>
// https://github.com/espressif/esp-idf/blob/v5.1.3/examples/system/ota/simple_ota_example/main/simple_ota_example.c

typedef void (*SimpleOtaOnLog_t)(char*);

uint8_t simple_ota_start_via_http(const char* firmwareUrl, SimpleOtaOnLog_t onLog);
