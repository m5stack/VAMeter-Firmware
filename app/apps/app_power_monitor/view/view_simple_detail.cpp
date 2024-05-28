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
using namespace SmoothUIToolKit;
using namespace SYSTEM::INPUTS;

static constexpr int _panel_width = 224;
static constexpr int _panel_height = 50;
static constexpr int _panel_gap = 8;
static constexpr int _panel_rounded = 16;
static constexpr int _value_ml = 20;
static constexpr int _unit_ml = 177;
// static const uint32_t _color_v = 0x0095FF;
// static const uint32_t _color_a = 0xFF0066;
// static const uint32_t _color_p = 0x009933;
// static const uint32_t _color_t = 0x191919;

void PmDataPage::goSimpleDetailPage()
{
    if (_data.is_simple_detail_page)
        return;

    _data.is_simple_detail_page = true;
    _data.bg_color.moveTo(AssetPool::GetColor().AppPowerMonitor.pageSimpleDetailBackground);

    // Add transitions
    _data.transition_list_buffer.resize(4);
    HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageSimpleDetailText);
    HAL::GetCanvas()->setTextSize(1);

    /* -------------------------------------------------------------------------- */
    /*                                   Voltage                                  */
    /* -------------------------------------------------------------------------- */
    _data.transition_list_buffer[0].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillSmoothRoundRect(transition->getValue().x,
                                              transition->getValue().y,
                                              _panel_width,
                                              _panel_height,
                                              _panel_rounded,
                                              AssetPool::GetColor().AppPowerMonitor.pageBusVoltage);

        // Value
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_36);
        HAL::GetCanvas()->setTextDatum(middle_left);

        _data.string_buffer = HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().busVoltage).value;
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _value_ml, transition->getValue().y + _panel_height / 2);

        // Unit
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_24);
        HAL::GetCanvas()->setTextDatum(middle_center);

        _data.string_buffer = HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().busVoltage).unit;
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _unit_ml, transition->getValue().y + _panel_height / 2);
    });

    /* -------------------------------------------------------------------------- */
    /*                                   Current                                  */
    /* -------------------------------------------------------------------------- */
    _data.transition_list_buffer[1].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillSmoothRoundRect(transition->getValue().x,
                                              transition->getValue().y,
                                              _panel_width,
                                              _panel_height,
                                              _panel_rounded,
                                              AssetPool::GetColor().AppPowerMonitor.pageShuntCurrent);

        // Value
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_36);
        HAL::GetCanvas()->setTextDatum(middle_left);

        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().shuntCurrent).value;
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _value_ml, transition->getValue().y + _panel_height / 2);

        // Unit
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_24);
        HAL::GetCanvas()->setTextDatum(middle_center);

        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().shuntCurrent).unit;
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _unit_ml, transition->getValue().y + _panel_height / 2);
    });

    /* -------------------------------------------------------------------------- */
    /*                                  Power/C/E                                 */
    /* -------------------------------------------------------------------------- */
    _data.transition_list_buffer[2].setUpdateCallback([&](Transition2D* transition) {
        /* ------------------------------ Update iuput ------------------------------ */
        if (Button::Side()->wasClicked())
        {
            _data.simple_page_special_line_index++;
            if (_data.simple_page_special_line_index > 2)
                _data.simple_page_special_line_index = 0;
            spdlog::info("switch to content {}", _data.simple_page_special_line_index);
        }

        if (Button::Encoder()->wasClicked())
        {
            spdlog::info("reset pm data");
            HAL::ResetPowerMonitorData();
        }

        /* --------------------------------- Render --------------------------------- */
        HAL::GetCanvas()->fillSmoothRoundRect(transition->getValue().x,
                                              transition->getValue().y,
                                              _panel_width,
                                              _panel_height,
                                              _panel_rounded,
                                              AssetPool::GetColor().AppPowerMonitor.pageBusPower);

        // Value
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_36);
        HAL::GetCanvas()->setTextDatum(middle_left);

        if (_data.simple_page_special_line_index == 0)
            _data.string_buffer = HAL::GetUnitAdaptatedPower(HAL::GetPowerMonitorData().busPower).value;
        else if (_data.simple_page_special_line_index == 1)
            _data.string_buffer = HAL::GetUnitAdaptatedCapacity(HAL::GetPowerMonitorData().capacity).value;
        else if (_data.simple_page_special_line_index == 2)
            _data.string_buffer = HAL::GetUnitAdaptatedEnergy(HAL::GetPowerMonitorData().energy).value;

        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _value_ml, transition->getValue().y + _panel_height / 2);

        // Unit
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_24);
        HAL::GetCanvas()->setTextDatum(middle_center);

        if (_data.simple_page_special_line_index == 0)
            _data.string_buffer = HAL::GetUnitAdaptatedPower(HAL::GetPowerMonitorData().busPower).unit;
        else if (_data.simple_page_special_line_index == 1)
            _data.string_buffer = HAL::GetUnitAdaptatedCapacity(HAL::GetPowerMonitorData().capacity).unit;
        else if (_data.simple_page_special_line_index == 2)
            _data.string_buffer = HAL::GetUnitAdaptatedEnergy(HAL::GetPowerMonitorData().energy).unit;

        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + _unit_ml, transition->getValue().y + _panel_height / 2);
    });

    /* -------------------------------------------------------------------------- */
    /*                                    Time                                    */
    /* -------------------------------------------------------------------------- */
    _data.transition_list_buffer[3].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillSmoothRoundRect(transition->getValue().x,
                                              transition->getValue().y,
                                              _panel_width,
                                              _panel_height,
                                              _panel_rounded,
                                              AssetPool::GetColor().AppPowerMonitor.pageTime);

        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_36);
        HAL::GetCanvas()->setTextDatum(middle_center);

        // spdlog::info("{}", HAL::GetPowerMonitorData().time);
        uint32_t seconds = HAL::GetPowerMonitorData().time / 1000;
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;
        uint32_t days = hours / 24;

        seconds = seconds % 60;
        minutes = minutes % 60;
        hours = hours % 24;

        _data.string_buffer = fmt::format("{}:{:02}:{:02}:{:02}", days, hours, minutes, seconds);
        HAL::GetCanvas()->drawString(_data.string_buffer.c_str(),
                                     transition->getValue().x + _panel_width / 2,
                                     transition->getValue().y + _panel_height / 2);
    });

    // Transition config
    for (int i = 0; i < _data.transition_list_buffer.size(); i++)
    {
        _data.transition_list_buffer[i].jumpTo(-240 + _panel_gap, _panel_gap + i * _panel_gap + i * _panel_height - 25);
        _data.transition_list_buffer[i].moveTo(0 + _panel_gap, _panel_gap + i * _panel_gap + i * _panel_height);
        _data.transition_list_buffer[i].setDelay(i * _panel_height);
        _data.transition_list_buffer[i].setTransitionPath(EasingPath::easeOutBack);
        _data.transition_list_buffer[i].setDuration(500);
    }
}

void PmDataPage::quitSimpleDetailPage()
{
    if (!_data.is_simple_detail_page)
        return;

    // Free and reset
    _data.is_simple_detail_page = false;
    _data.transition_list_buffer.clear();
    reset();
}
