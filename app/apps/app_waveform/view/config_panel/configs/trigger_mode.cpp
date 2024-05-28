/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../config_panel.h"
#include <cstddef>
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
static constexpr int _panel_y = 73;
static constexpr int _panel_w = 222;
static constexpr int _panel_h = 82;
static constexpr int _panel_r = 11;
static uint32_t _color_container = 0xCADED5;
static uint32_t _color_primary = 0x035E36;
static uint32_t _color_on_primary = 0xFFFFFF;

void ConfigTrigerMode::onInit()
{
    _trigger_mode_label_list.push_back(AssetPool::GetText().AppWaveform_Mode_Manual);
    _trigger_mode_label_list.push_back(AssetPool::GetText().AppWaveform_Mode_VHL);
    _trigger_mode_label_list.push_back(AssetPool::GetText().AppWaveform_Mode_VLL);
    _trigger_mode_label_list.push_back(AssetPool::GetText().AppWaveform_Mode_CHL);
    _trigger_mode_label_list.push_back(AssetPool::GetText().AppWaveform_Mode_CLL);
}

void ConfigTrigerMode::onPopOut()
{
    constexpr int delay = 60;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
}

void ConfigTrigerMode::onHide()
{
    constexpr int delay = 60;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 50, delay + 0, delay + 0, delay + 0);
}

void ConfigTrigerMode::onRender()
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
    HAL::GetCanvas()->drawString(AssetPool::GetText().AppWaveform_Config_TriggerMode, frame.x + 11, frame.y + 22);

    // Value
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->drawString(_trigger_mode_label_list[static_cast<size_t>(_config->trigger_mode)].c_str(),
                                 frame.x + frame.w / 2,
                                 frame.y + frame.h - 25);
}

void ConfigTrigerMode::onUpdate()
{
    if (!isSelected())
        return;

    // Triger mode config
    if (Encoder::WasMoved())
    {
        int new_trigger_mode = 0;
        if (Encoder::GetDirection() < 0)
        {
            new_trigger_mode = static_cast<int>(_config->trigger_mode) + 1;
            if (new_trigger_mode >= trigger_mode_num)
                new_trigger_mode = 0;
        }
        else
        {
            new_trigger_mode = static_cast<int>(_config->trigger_mode) - 1;
            if (new_trigger_mode < 0)
                new_trigger_mode = trigger_mode_num - 1;
        }
        _config->trigger_mode = static_cast<TriggerMode_t>(new_trigger_mode);

        // Set to default threshold
        if (_config->trigger_mode == trigger_mode_v_above_level || _config->trigger_mode == trigger_mode_v_under_level)
            _config->threshold = 1.0f;
        else if (_config->trigger_mode == trigger_mode_a_above_level || _config->trigger_mode == trigger_mode_a_under_level)
            _config->threshold = 0.3f;
    }
}
