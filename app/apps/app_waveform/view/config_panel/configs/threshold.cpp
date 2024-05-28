/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../config_panel.h"
#include <cstdint>
#include <mooncake.h>
#include "../../../../../hal/hal.h"
#include "../../../triggers/triggers.h"
#include "../../../../utils/system/system.h"
#include "../../../../../assets/assets.h"
#include "core/easing_path/easing_path.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"

using namespace SYSTEM::INPUTS;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x = 9;
static constexpr int _panel_y = 175;
static constexpr int _panel_w = 222;
static constexpr int _panel_h = 44;
static constexpr int _panel_r = 11;
static uint32_t _color_container = 0xCADED5;
static uint32_t _color_primary = 0x035E36;
static uint32_t _color_on_primary = 0xFFFFFF;

void ConfigThreshold::onInit() {}

void ConfigThreshold::onPopOut()
{
    constexpr int delay = 120;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 0, delay + 0, delay + 50, delay + 0);
}

void ConfigThreshold::onHide()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 50, delay + 0, delay + 0, delay + 0);
}

void ConfigThreshold::onRender()
{
    if (isHidden())
        return;

    auto frame = getTransition().getValue();

    // Panel
    if (isSelected())
    {
        HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_primary);
        HAL::GetCanvas()->setTextColor(_color_on_primary);
    }
    else
    {
        HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, _color_container);
        HAL::GetCanvas()->setTextColor(_color_primary);
    }

    // Title
    AssetPool::LoadFont16(HAL::GetCanvas());
    HAL::GetCanvas()->setTextDatum(middle_left);
    HAL::GetCanvas()->drawString(AssetPool::GetText().AppWaveform_Config_Threshold, frame.x + 11, frame.y + frame.h / 2);

    // Value
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->setTextDatum(middle_center);

    if (IsTriggerModeHasThreshold(_config->trigger_mode))
    {
        std::string string_buffer;
        if (IsTriggerModeChannelV(_config->trigger_mode))
        {
            string_buffer = HAL::GetUnitAdaptatedVoltage(_config->threshold).value;
            string_buffer += HAL::GetUnitAdaptatedVoltage(_config->threshold).unit;
        }
        else
        {
            string_buffer = HAL::GetUnitAdaptatedCurrent(_config->threshold).value;
            string_buffer += HAL::GetUnitAdaptatedCurrent(_config->threshold).unit;
        }
        HAL::GetCanvas()->drawString(string_buffer.c_str(), frame.x + 161, frame.y + frame.h / 2);
    }
    else
        HAL::GetCanvas()->drawString("~", frame.x + 161, frame.y + frame.h / 2);
}

static constexpr float _threshold_v_max = 12.0f;
static constexpr float _threshold_v_min = 0.0f;
static constexpr float _threshold_v_step = 0.1f;
static constexpr float _threshold_a_max = 5.0f;
static constexpr float _threshold_a_min = 0.0f;
static constexpr float _threshold_a_step_ma = 0.005f;
static constexpr float _threshold_a_step_ua = 0.00005f;
static constexpr float _threshold_a_ua_step_line = 0.001f;

void ConfigThreshold::onUpdate()
{
    if (!isSelected())
        return;

    if (!IsTriggerModeHasThreshold(_config->trigger_mode))
        return;

    // Triger threshold config
    if (Encoder::WasMoved())
    {
        auto new_threshold = _config->threshold;

        // Channel V
        if (IsTriggerModeChannelV(_config->trigger_mode))
        {
            if (Encoder::GetDirection() < 0)
                new_threshold += _threshold_v_step;
            else
                new_threshold -= _threshold_v_step;

            if (new_threshold > _threshold_v_max)
                new_threshold = _threshold_v_max;
            else if (new_threshold < _threshold_v_min)
                new_threshold = _threshold_v_min;
        }

        // Channel A
        else
        {
            if (Encoder::GetDirection() < 0)
            {
                if (new_threshold < _threshold_a_ua_step_line)
                    new_threshold += _threshold_a_step_ua;
                else
                    new_threshold += _threshold_a_step_ma;
            }
            else
            {
                if (new_threshold < _threshold_a_ua_step_line)
                    new_threshold -= _threshold_a_step_ua;
                else
                {
                    if (new_threshold - _threshold_a_step_ma < _threshold_a_ua_step_line)
                        new_threshold -= _threshold_a_step_ua;
                    else
                        new_threshold -= _threshold_a_step_ma;
                }
            }

            if (new_threshold > _threshold_a_max)
                new_threshold = _threshold_a_max;
            else if (new_threshold < _threshold_a_min)
                new_threshold = _threshold_a_min;
        }

        _config->threshold = new_threshold;
    }
}
