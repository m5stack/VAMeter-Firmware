/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "app_hal_test.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../utils/qrcode/qrcode.h"
#include "apps/utils/system/inputs/button/button.h"
#include <mooncake.h>
#include <vector>
#include <string>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace QRCODE;

// Theme color
constexpr static uint32_t _theme_color = 0xA9D8FF;
void* AppHal_test_Packer::getCustomData() { return (void*)(&_theme_color); }

// Icon
void* AppHal_test_Packer::getAppIcon() { return nullptr; }

// Like setup()...
void AppHal_test::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(_theme_color);
    HAL::CanvasUpdate();

    // RenderQRCode(10, 10, "http://192.168.31.233/啊?/LLLL#?sasdMacs98?69:89:99:90");

    // RenderQRCode(120, 120, "http://192.168.31.233/aaogswhg/sh/sderh/sh/sh/sh/sh/dh/sdh/s/hse/hg/seh/esh/se/hse/hs/hs/h/");
    // HAL::CanvasUpdate();

    // while (1)
    //     ;
}

// Like loop()...
void AppHal_test::onRunning()
{
    std::vector<std::string> option_list = {"quit", "web server"};

    while (1)
    {
        auto shit = SelectMenuPage::CreateAndWaitResult("HAL TEST", option_list);

        if (shit <= 0)
            break;

        if (shit == 1)
        {
            HAL::StartWebServer(OnLogPageRender);

            spdlog::info("{}", HAL::GetSystemConfigUrl());
        }

        while (1)
        {
            // spdlog::info("{}", HAL::GetApStaNum());
            // HAL::Delay(500);
            HAL::FeedTheDog();

            HAL::Delay(100);
            Button::Update();
            if (Button::Side()->wasClicked())
                break;
        }

        HAL::StopWebServer();
    }

    destroyApp();

    Button::Update();
    if (Button::Side()->wasClicked())
        destroyApp();
}

void AppHal_test::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
