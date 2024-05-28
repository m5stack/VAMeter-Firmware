/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "misc.h"
#include "../../inputs/inputs.h"
#include "../../../../../assets/assets.h"
#include <mooncake.h>
#include <cstdint>
#include <string>

using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

void SYSTEM::UI::OnLogPageRender(const std::string& log, bool pushScreen, bool clear)
{
    if (clear)
    {
        HAL::GetCanvas()->fillScreen(AssetPool::GetColor().AppSettings.background);
        HAL::GetCanvas()->setBaseColor(AssetPool::GetColor().AppSettings.background);

        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->setTextDatum(top_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionText,
                                       AssetPool::GetColor().AppSettings.background);
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_16);

        HAL::GetCanvas()->setCursor(0, 10);
        HAL::GetCanvas()->setTextScroll(true);
    }

    HAL::GetCanvas()->print(log.c_str());

    if (pushScreen)
        HAL::CanvasUpdate();
}

static const std::string _tag_progress_bar = "<PB>";
static constexpr int _progress_bar_padding = 10;

void SYSTEM::UI::OnOtaLogPageRender(const std::string& log, bool pushScreen, bool clear)
{
    if (clear)
    {
        HAL::GetCanvas()->fillScreen(AssetPool::GetColor().AppSettings.background);
        HAL::GetCanvas()->setBaseColor(AssetPool::GetColor().AppSettings.background);

        HAL::GetCanvas()->setCursor(0, 10);
        HAL::GetCanvas()->setTextScroll(true);
    }

    // If is progress bar tag
    size_t pos = log.find(_tag_progress_bar);
    if (log.find(_tag_progress_bar) != std::string::npos)
    {
        int percentage = std::stoi(log.substr(pos + _tag_progress_bar.length()));

        HAL::GetCanvas()->fillScreen(AssetPool::GetColor().AppSettings.background);
        HAL::GetCanvas()->fillSmoothRoundRect(_progress_bar_padding,
                                              _progress_bar_padding,
                                              (HAL::GetCanvas()->width() - _progress_bar_padding * 2) * percentage / 100,
                                              HAL::GetCanvas()->height() - _progress_bar_padding * 2,
                                              24,
                                              AssetPool::GetColor().AppSettings.selector);

        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->setTextDatum(top_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionText);
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);

        std::string label = log.substr(pos + _tag_progress_bar.length());
        label += "%";
        HAL::GetCanvas()->drawString(label.c_str(), HAL::GetCanvas()->width() / 2, 48);

        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
        HAL::GetCanvas()->drawString("UPGRADING", HAL::GetCanvas()->width() / 2, 128);
    }

    else
    {
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->setTextDatum(top_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionText,
                                       AssetPool::GetColor().AppSettings.background);
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_16);

        HAL::GetCanvas()->print(log.c_str());
    }

    if (pushScreen)
        HAL::CanvasUpdate();
};
