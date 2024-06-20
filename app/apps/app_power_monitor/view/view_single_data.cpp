/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "view.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include <smooth_ui_toolkit.h>
#include <mooncake.h>
#include <cstdint>

using namespace VIEWS;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

/* -------------------------------------------------------------------------- */
/*                              Single data page                              */
/* -------------------------------------------------------------------------- */
PmDataPage::PmDataPage()
{
    _data.spinner_icon = new LGFX_Sprite(HAL::GetCanvas());
    _data.spinner_icon->createSprite(37, 37);
    _data.spinner_icon->pushImage(0, 0, 37, 37, AssetPool::GetStaticAsset()->Image.AppPowerMonitor.spinner_icon);
}

PmDataPage::~PmDataPage() { delete _data.spinner_icon; }

void PmDataPage::reset()
{
    // Transition reset
    _data.value_postion.setTransitionPath(EasingPath::easeOutBack);
    _data.value_postion.setDuration(600);
    _data.value_postion.jumpTo(-20, 12 - 25);
    _data.value_postion.moveTo(220, 12);

    _data.label_postion.setDelay(50);
    _data.label_postion.setTransitionPath(EasingPath::easeOutBack);
    _data.label_postion.setDuration(600);
    _data.label_postion.jumpTo(-20, 110);
    _data.label_postion.moveTo(218, 110);

    _data.panel_postion.setDelay(400);
    _data.panel_postion.setDuration(600);
    _data.panel_postion.jumpTo(240, 240);
    _data.panel_postion.moveTo(50, 175);

    _data.mode_postion.setDelay(150);
    _data.mode_postion.setTransitionPath(EasingPath::easeOutBack);
    _data.mode_postion.setDuration(600);
    _data.mode_postion.jumpTo(-20, 140);
    _data.mode_postion.moveTo(217, 140);

    _data.bg_color.setDuration(400);
    if (_data.is_just_create)
    {
        _data.is_just_create = false;
        _data.bg_color.jumpTo(_config.themeColor);
    }
    else
    {
        _data.bg_color.moveTo(_config.themeColor);
    }
}

void PmDataPage::update(uint32_t currentTime)
{
    if (currentTime - _data.time_count > _config.updateInterval)
    {
        Button::Update();

        // Page simple detail or all detail page
        if (_data.is_simple_detail_page || _data.is_all_detail_page)
        {
            _data.bg_color.update(currentTime);
            HAL::GetCanvas()->fillScreen(Rgb2Hex(_data.bg_color.getValue()));
            for (auto& i : _data.transition_list_buffer)
                i.update(currentTime);
            HAL::CanvasUpdate();
        }
        // Single data page
        else
        {
            _data.value_postion.update(currentTime);
            _data.label_postion.update(currentTime);
            _data.panel_postion.update(currentTime);
            _data.mode_postion.update(currentTime);
            _data.bg_color.update(currentTime);
            _render();
        }

        if (Button::Encoder()->wasHold())
        {
            HAL::SetBaseRelay(!HAL::GetBaseRelayState());
        }

        // Check quit
        if (Button::Side()->wasHold())
        {
            _data.want_to_quit = true;
        }

        _data.time_count = currentTime;
    }
}

void PmDataPage::_update_mode_label()
{
    if (HAL::IsPowerMonitorInLowCurrentMode() && !_data.is_lc_mode)
    {
        _data.mode_postion.moveTo(212, 140);
        _data.is_lc_mode = HAL::IsPowerMonitorInLowCurrentMode();
    }
    else if (!HAL::IsPowerMonitorInLowCurrentMode() && _data.is_lc_mode)
    {
        _data.mode_postion.moveTo(-20, 140);
        _data.is_lc_mode = HAL::IsPowerMonitorInLowCurrentMode();
    }
}

void PmDataPage::_render()
{
    // HAL::GetCanvas()->fillScreen(_config.themeColor);
    // HAL::GetCanvas()->setTextColor(TFT_WHITE, _config.themeColor);
    HAL::GetCanvas()->fillScreen(Rgb2Hex(_data.bg_color.getValue()));
    HAL::GetCanvas()->setTextColor(TFT_WHITE, Rgb2Hex(_data.bg_color.getValue()));
    HAL::GetCanvas()->setTextSize(1);

    // Value
    _config.getValueCallback(_data.string_buffer);
    HAL::GetCanvas()->setTextDatum(top_right);
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
    HAL::GetCanvas()->drawString(
        _data.string_buffer.c_str(), _data.value_postion.getValue().x, _data.value_postion.getValue().y);

    // Label
    _config.getLabelCallback(_data.string_buffer);
    // HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->drawString(
        _data.string_buffer.c_str(), _data.label_postion.getValue().x, _data.label_postion.getValue().y);

    // Mode
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_16);
    // HAL::GetCanvas()->drawString("Precision Mode", _data.mode_postion.getValue().x, _data.mode_postion.getValue().y);
    if (_config.showLcModeLabel)
    {
        if (HAL::IsPowerMonitorInLowCurrentMode())
            HAL::GetCanvas()->drawString("DIV(2.5μA)", _data.mode_postion.getValue().x, _data.mode_postion.getValue().y);
        else
            HAL::GetCanvas()->drawString("DIV(250μA)", _data.mode_postion.getValue().x, _data.mode_postion.getValue().y);
    }

    // Spinner
    _data.spinner_icon->pushRotateZoom(
        _data.label_postion.getValue().x - 190, _data.label_postion.getValue().y + 86, _data.spinner_angle, 1.0, 1.0, 0);
    _data.spinner_angle += 2;

    // Panel
    HAL::GetCanvas()->pushImage(
        _data.panel_postion.getValue().x, _data.panel_postion.getValue().y, 190, 65, (uint16_t*)_config.panelImage);

#ifdef ESP_PLATFORM
    // HAL::RenderFpsPanel();
#endif
    HAL::CanvasUpdate();
}
