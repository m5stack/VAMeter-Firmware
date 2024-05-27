/**
 * @file hal_vabase.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-01-24
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>

static bool _base_relay_state = false;

void HAL_VAMeter::_vabase_init()
{
    spdlog::info("vabase init");

    gpio_reset_pin((gpio_num_t)HAL_PIN_BASE_RELAY_CTRL);
    gpio_set_direction((gpio_num_t)HAL_PIN_BASE_RELAY_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_BASE_RELAY_CTRL, GPIO_PULLUP_PULLDOWN);

    setBaseRelay(false);
}

void HAL_VAMeter::setBaseRelay(bool state)
{
    // High close
    _base_relay_state = state;
    gpio_set_level((gpio_num_t)HAL_PIN_BASE_RELAY_CTRL, _base_relay_state);
}

bool HAL_VAMeter::getBaseRelayState() { return _base_relay_state; }

void HAL_VAMeter::baseGroveStartTest()
{
    gpio_reset_pin((gpio_num_t)HAL_PIN_BASE_GROVE_IOA);
    gpio_set_direction((gpio_num_t)HAL_PIN_BASE_GROVE_IOA, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_BASE_GROVE_IOA, GPIO_PULLUP_ONLY);
    gpio_reset_pin((gpio_num_t)HAL_PIN_BASE_GROVE_IOB);
    gpio_set_direction((gpio_num_t)HAL_PIN_BASE_GROVE_IOB, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_BASE_GROVE_IOB, GPIO_PULLUP_ONLY);
}

void HAL_VAMeter::baseGroveStopTest()
{
    gpio_reset_pin((gpio_num_t)HAL_PIN_BASE_GROVE_IOA);
    gpio_reset_pin((gpio_num_t)HAL_PIN_BASE_GROVE_IOB);
}

bool HAL_VAMeter::baseGroveGetIoALevel() { return (bool)gpio_get_level((gpio_num_t)HAL_PIN_BASE_GROVE_IOA); }

bool HAL_VAMeter::baseGroveGetIoBLevel() { return (bool)gpio_get_level((gpio_num_t)HAL_PIN_BASE_GROVE_IOB); }
