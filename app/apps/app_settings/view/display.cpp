/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /display
void AppSettings::_on_page_display()
{
    spdlog::info("on page display");

    int selected_index = 0;
    while (1)
    {
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppSettings_Option_Brightness);
        options.push_back(AssetPool::GetText().AppSettings_Option_Orientation);
        options.push_back(AssetPool::GetText().AppSettings_Option_RefreshRate);
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppSettings_Option_Display, options, selected_index, &_data.select_page_theme);

        // Quit
        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        if (selected_index == 0)
        {
            if (CreatBrightnessSettingPage())
            {
                HAL::GetSystemConfig().brightness = HAL::GetDisplay()->getBrightness();
                HAL::SaveSystemConfig();
            }
        }

        else if (selected_index == 1)
            _on_page_orientation();

        else if (selected_index == 2)
            _on_page_refresh_rate();
    }
}

// Page: /display/orientation
void AppSettings::_on_page_orientation()
{
    spdlog::info("on page orientation");
    auto history_orientation = HAL::GetDisplay()->getRotation();

    int selected_index = history_orientation;
    while (1)
    {
        std::vector<std::string> options = {"0", "90", "180", "270"};
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppSettings_Option_Orientation, options, selected_index, &_data.select_page_theme);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        if (selected_index == 0)
            HAL::GetDisplay()->setRotation(0);
        else if (selected_index == 1)
            HAL::GetDisplay()->setRotation(1);
        else if (selected_index == 2)
            HAL::GetDisplay()->setRotation(2);
        else if (selected_index == 3)
            HAL::GetDisplay()->setRotation(3);

        break;
    }

    if (history_orientation != HAL::GetDisplay()->getRotation())
    {
        HAL::GetSystemConfig().orientation = HAL::GetDisplay()->getRotation();
        // _data.is_changed = true;
        HAL::SaveSystemConfig();
    }
}

// Page: /display/refresh_rate
void AppSettings::_on_page_refresh_rate()
{
    spdlog::info("on page refresh rate");
    auto history_refresh_rate = HAL::GetSystemConfig().highRefreshRate;

    int selected_index = HAL::GetSystemConfig().highRefreshRate ? 1 : 0;
    while (1)
    {
        // std::vector<std::string> options = {" - Normal", " - High", " - Back"};
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppSettings_Option_Normal);
        options.push_back(AssetPool::GetText().AppSettings_Option_High);
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppSettings_Option_RefreshRate, options, selected_index, &_data.select_page_theme);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        if (selected_index == 0)
            HAL::GetSystemConfig().highRefreshRate = false;
        else if (selected_index == 1)
            HAL::GetSystemConfig().highRefreshRate = true;

        break;
    }

    if (history_refresh_rate != HAL::GetSystemConfig().highRefreshRate)
    {
        HAL::SaveSystemConfig();
        if (CreateConfirmPage(AssetPool::GetText().Misc_RebootNow, true, &_data.select_page_theme))
        {
            HAL::Reboot();
        }
    }
}
