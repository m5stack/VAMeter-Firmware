/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_files.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

void AppFiles::_on_page_msc_mode()
{
    spdlog::info("on page msc mode");

    // Enter
    HAL::StartMscMode();

    HAL::GetCanvas()->fillScreen(AssetPool::GetColor().AppFile.background);

    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->setTextDatum(top_center);
    HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppFile.optionText, AssetPool::GetColor().AppFile.background);

    // HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
    AssetPool::LoadFont72(HAL::GetCanvas());
    HAL::GetCanvas()->drawString("MSC", 120, 30);

    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
    HAL::GetCanvas()->drawString("MODE", 120, 110);

    // {
    //     AssetPool::LoadFont24(HAL::GetCanvas());
    //     auto string_buffer = spdlog::fmt_lib::format("{} ->", AssetPool::GetText().Misc_Text_ClickToQuit);
    //     HAL::GetCanvas()->drawString(string_buffer.c_str(), 120, 200);
    // }

    HAL::CanvasUpdate();

    while (1)
    {
        HAL::FeedTheDog();
        HAL::Delay(50);

        Button::Update();
        if (Button::Side()->wasHold())
            break;
    }

    // Stop
    HAL::StopMscMode();
}
