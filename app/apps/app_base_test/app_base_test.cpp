/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "app_base_test.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// Theme color
constexpr static uint32_t _theme_color = 0x008CFF;
void* AppBase_test_Packer::getCustomData() { return (void*)(&_theme_color); }

// Icon
void* AppBase_test_Packer::getAppIcon() { return nullptr; }

// Like setup()...
void AppBase_test::onResume()
{
    spdlog::info("{} onResume", getAppName());
}

// Like loop()...
void AppBase_test::onRunning()
{
    // if (HAL::Millis() - _data.time_count > 500)
    // {
    //     HAL::SetBaseRelay(!HAL::GetBaseRelayState());
    //     spdlog::info("{}", HAL::GetBaseRelayState());
    //     _data.time_count = HAL::Millis();
    // }

    if (Button::Encoder()->wasClicked())
    {
        spdlog::info("????");
        HAL::SetBaseRelay(!HAL::GetBaseRelayState());
    }


    // Check quit
    Button::Update();
    if (Button::Side()->isHolding())
        destroyApp();
}

void AppBase_test::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    HAL::SetBaseRelay(false);
}
