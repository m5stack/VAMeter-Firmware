/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../hal_vameter.h"
#include "../hal_config.h"
#include "esp_system.h"
#include <mooncake.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_task_wdt.h>
// Refs:
// https://github.com/espressif/esp-idf/blob/v5.2.1/examples/system/task_watchdog/main/task_watchdog_example_main.c
// https://docs.espressif.com/projects/esp-idf/en/v5.2.1/esp32/api-reference/system/wdts.html

void HAL_VAMeter::_watch_dog_init()
{
    spdlog::info("watch dog init");

    // // Unsubscribes idle tasks
    // esp_task_wdt_deinit();

    // Init twdt
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = 8000,
        // .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Bitmask of all cores
        .idle_core_mask = 1 << 1, // Only IDLE1
        .trigger_panic = true,
    };
    ESP_ERROR_CHECK(esp_task_wdt_init(&twdt_config));

    // Subscribes main task
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    ESP_ERROR_CHECK(esp_task_wdt_status(NULL));

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // int b = 2;
    // while (1)
    // {
    //     // delay(5);
    //     int a = b * b;
    //     feedTheDog();
    // }
}

void HAL_VAMeter::feedTheDog() { esp_task_wdt_reset(); }

void HAL_VAMeter::reboot() { esp_restart(); }
