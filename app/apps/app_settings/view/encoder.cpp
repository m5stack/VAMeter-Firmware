/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "../../utils/easter_egg/easter_egg.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /encoder
void AppSettings::_on_page_encoder()
{
    spdlog::info("on page encoder");

    auto history_reverse_encoder = HAL::GetSystemConfig().reverseEncoder;
    int selected_index = HAL::GetSystemConfig().reverseEncoder ? 1 : 0;
    bool hit = false;

    while (1)
    {
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppSettings_Option_Forward);
        options.push_back(AssetPool::GetText().AppSettings_Option_Reverse);
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppSettings_Option_Encoder, options, selected_index, &_data.select_page_theme);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
        {
            hit = true;
            break;
        }

        else if (selected_index == 0)
            HAL::GetSystemConfig().reverseEncoder = false;
        else if (selected_index == 1)
            HAL::GetSystemConfig().reverseEncoder = true;

        break;
    }

    // Check save
    if (history_reverse_encoder != HAL::GetSystemConfig().reverseEncoder)
    {
        HAL::SaveSystemConfig();
    }

    if (hit)
    {
        _data.mystery_count++;
        if (_data.mystery_count > 3)
        {
            _data.mystery_count = 0;
            EasterEgg::CreateEasterEggMenu();
        }
    }
    else
        _data.mystery_count = 0;
}
