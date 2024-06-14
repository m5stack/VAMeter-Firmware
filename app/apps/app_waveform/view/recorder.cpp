/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "lgfx/v1/misc/enum.hpp"
#include "view.h"
#include "../../../hal/hal.h"
#include "../triggers/triggers.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include <array>
#include <cmath>
#include <mooncake.h>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

using namespace SmoothUIToolKit;
using namespace VIEWS;
using namespace SYSTEM::INPUTS;

WaveFormRecorder::~WaveFormRecorder()
{
    if (_data.config_panel != nullptr)
        delete _data.config_panel;
    if (_data.trigger != nullptr)
        delete _data.trigger;

    // Destroy recorder
    HAL::DestroyVaRecorder();
}

void WaveFormRecorder::init()
{
    _data.config_panel = new ConfigPanel;
    _data.config_panel->init();

    _data.string_y_offset = AssetPool::IsLocaleEn() ? 2 : 3;
    Encoder::Reset();
}

/* -------------------------------------------------------------------------- */
/*                                   Update                                   */
/* -------------------------------------------------------------------------- */
void WaveFormRecorder::update()
{
    // Update inputs
    Button::Update();
    Encoder::Update();

    switch (_data.state)
    {
    case state_idle:
        _update_state_idle();
        break;
    case state_waiting_trigger:
        _update_state_waiting_trigger();
        break;
    case state_recording:
        _update_state_recording();
        break;
    case state_saving:
        _update_state_saving();
        break;
    default:
        break;
    }

    if (Button::Encoder()->wasHold())
    {
      HAL::SetBaseRelay(!HAL::GetBaseRelayState());
    }

    // Check quit
    if (Button::Side()->wasHold())
        _data.want_to_quit = true;
}

// Override to add y zoom with threshold line
void WaveFormRecorder::_update_input()
{
    Waveform::_update_pm_data();
    Waveform::_update_chart_x_zoom();

    // Get a scaled buffer to avoid redundant float operation
    if (IsTriggerModeHasThreshold(getConfig().trigger_mode) && !IsTriggerModeChannelV(getConfig().trigger_mode))
        _data.threshold_a_scaled_buffer = getConfig().threshold * Waveform::_get_pm_data_a_scale();

    // Update chary y zoom
    if (IsTriggerModeHasThreshold(getConfig().trigger_mode))
    {
        // V
        if (IsTriggerModeChannelV(getConfig().trigger_mode))
            Waveform::_update_chart_y_zoom_with_third_value(getConfig().threshold, 0);
        // A
        else
            Waveform::_update_chart_y_zoom_with_third_value(0, _data.threshold_a_scaled_buffer);
    }
    else
        Waveform::_update_chart_y_zoom();
}

void WaveFormRecorder::_update_chart()
{
    _update_input();
    Waveform::_update_transition();
}

void WaveFormRecorder::_update_state_idle()
{
    // Update waveform
    if (!_data.config_panel->isPoppedOut())
    {
        _update_chart();
    }

    // Open or close config panel
    if (Button::Side()->wasClicked() && _data.config_panel->isTransitionFinish())
    {
        if (_data.config_panel->isHidden())
        {
            enableChartXUpdate(false);
            _data.config_panel->popOut();
        }
        else
        {
            enableChartXUpdate(true);
            _data.config_panel->hide();
        }
    }
    _data.config_panel->update(HAL::Millis());

    // Start recording
    if (!_data.config_panel->isPoppedOut() && Button::Encoder()->wasClicked())
    {
        _handle_start_recording();
        spdlog::info("jump to state_waiting_trigger");
        _data.state = state_waiting_trigger;
    }

    _handle_render();
}

void WaveFormRecorder::_update_state_waiting_trigger()
{
    _update_chart();
    _handle_render();

    if (HAL::IsVaRecorderRecording())
    {
        spdlog::info("jump to state_recording");
        _data.state = state_recording;
        _data.recording_start_time_count = HAL::Millis();
    }
}

void WaveFormRecorder::_update_state_recording()
{
    _update_chart();
    _handle_render();

    if (!HAL::IsVaRecorderRecording())
    {
        spdlog::info("jump to state_saving");
        _data.state = state_saving;

        // // Render tag
        // HAL::GetCanvas()->fillSmoothRoundRect(60, 95, 120, 50, 10, AssetPool::GetColor().AppFile.selector);

        // HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppFile.optionText, AssetPool::GetColor().AppFile.selector);
        // HAL::GetCanvas()->setTextDatum(middle_center);
        // AssetPool::LoadFont24(HAL::GetCanvas());
        // HAL::GetCanvas()->drawString(
        //     AssetPool::GetText().AppWaveform_Saving, HAL::GetCanvas()->width() / 2, HAL::GetCanvas()->height() / 2);

        HAL::GetCanvas()->fillRectAlpha(0, 100, 240, 40, 160, TFT_BLACK);
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(TFT_WHITE);
        AssetPool::LoadFont24(HAL::GetCanvas());
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppWaveform_Saving, 120, 120);

        HAL::CanvasUpdate();
    }
}

void WaveFormRecorder::_update_state_saving()
{
    if (!HAL::IsVaRecorderSaving())
    {
        _data.has_finished_recording = true;
        spdlog::info("jump to state_idle");
        _data.state = state_idle;
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void WaveFormRecorder::_handle_render()
{
    // Render waveform
    if (!_data.config_panel->isPoppedOut())
    {
        Waveform::_update_render(false, true, false);
        _render_mode_icon();
        _render_threshold_line();
        _render_rec_state_label();
        Waveform::_render_x_scales_notice();
    }

    // Render config panel
    if (!_data.config_panel->isHidden())
    {
        _data.config_panel->render();
    }

    HAL::CanvasUpdate();
}

static const std::vector<std::string> _trigger_mode_icon_list = {
    "M",
    "V.H.L",
    "V.L.L",
    "C.H.L",
    "C.L.L",
};
static constexpr int _trigger_mode_icon_margin_top = 145 + 23 + 3;
static constexpr int _trigger_mode_icon_margin_left = 3;

void WaveFormRecorder::_render_mode_icon()
{
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
    // AssetPool::LoadFont14(HAL::GetCanvas());

    HAL::GetCanvas()->setTextDatum(top_left);
    HAL::GetCanvas()->setTextColor(TFT_WHITE, AssetPool::GetColor().AppWaveform.colorTriggerModeIcon);

    auto panel_width =
        HAL::GetCanvas()->textWidth(_trigger_mode_icon_list[static_cast<size_t>(getConfig().trigger_mode)].c_str());
    panel_width += 10;
    HAL::GetCanvas()->fillSmoothRoundRect(_trigger_mode_icon_margin_left,
                                          _trigger_mode_icon_margin_top,
                                          panel_width,
                                          23,
                                          6,
                                          AssetPool::GetColor().AppWaveform.colorTriggerModeIcon);

    HAL::GetCanvas()->drawString(_trigger_mode_icon_list[static_cast<size_t>(getConfig().trigger_mode)].c_str(),
                                 _trigger_mode_icon_margin_left + 5,
                                 _trigger_mode_icon_margin_top + 4);
}

static constexpr int _threshold_line_tag_margin_left = 90;

void WaveFormRecorder::_render_threshold_line()
{
    if (!IsTriggerModeHasThreshold(getConfig().trigger_mode))
        return;

    int threshold_line_chart_y = 0;

    // V
    if (IsTriggerModeChannelV(getConfig().trigger_mode))
    {
        // Get chart y
        threshold_line_chart_y = Waveform::_chart_props.chart_v.getChartPoint(0, getConfig().threshold).y;
        // Get label
        Waveform::_get_string_buffer() = HAL::GetUnitAdaptatedVoltage(getConfig().threshold).value;
        Waveform::_get_string_buffer() += HAL::GetUnitAdaptatedVoltage(getConfig().threshold).unit;
    }
    // A
    else
    {
        threshold_line_chart_y = Waveform::_chart_props.chart_a.getChartPoint(0, _data.threshold_a_scaled_buffer).y;
        Waveform::_get_string_buffer() = HAL::GetUnitAdaptatedCurrent(getConfig().threshold).value;
        Waveform::_get_string_buffer() += HAL::GetUnitAdaptatedCurrent(getConfig().threshold).unit;
    }

    // Render line
    for (int i = 0; i < 3; i++)
        HAL::GetCanvas()->drawFastHLine(
            0, threshold_line_chart_y + i, HAL::GetCanvas()->width(), AssetPool::GetColor().AppWaveform.colorThresholdLine);

    // Render tag
    HAL::GetCanvas()->fillSmoothRoundRect(_threshold_line_tag_margin_left,
                                          threshold_line_chart_y,
                                          70,
                                          18,
                                          3,
                                          AssetPool::GetColor().AppWaveform.colorThresholdLine);

    HAL::GetCanvas()->setTextDatum(top_center);
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);

    HAL::GetCanvas()->setTextColor(TFT_WHITE, AssetPool::GetColor().AppWaveform.colorThresholdLine);
    HAL::GetCanvas()->drawString(
        Waveform::_get_string_buffer().c_str(), _threshold_line_tag_margin_left + 37, threshold_line_chart_y + 2);
}

static constexpr int _state_label_mergin_top_waiting = 145;
static constexpr int _state_label_mergin_left_waiting = 3;
static constexpr int _state_label_mergin_top_recording = 145;
static constexpr int _state_label_mergin_left_recording = 3;

void WaveFormRecorder::_render_rec_state_label()
{
    // Waiting trigger
    if (_data.state == state_waiting_trigger)
    {
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
        // AssetPool::LoadFont14(HAL::GetCanvas());

        HAL::GetCanvas()->setTextDatum(top_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppFile.optionText,
                                       AssetPool::GetColor().AppWaveform.colorStateLabelWaiting);

        HAL::GetCanvas()->fillSmoothRoundRect(_state_label_mergin_left_waiting,
                                              _state_label_mergin_top_waiting,
                                              75,
                                              23,
                                              6,
                                              AssetPool::GetColor().AppWaveform.colorStateLabelWaiting);

        HAL::GetCanvas()->drawString("WAITING", _state_label_mergin_left_waiting + 5, _state_label_mergin_top_waiting + 4);
        // HAL::GetCanvas()->drawString(AssetPool::GetText().AppWaveform_Waitting,
        //                              _state_label_mergin_left_waiting + 5,
        //                              _state_label_mergin_top_waiting + 2);
    }

    else if (_data.state == state_recording)
    {
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
        // AssetPool::LoadFont14(HAL::GetCanvas());

        HAL::GetCanvas()->setTextDatum(top_left);
        HAL::GetCanvas()->setTextColor(TFT_WHITE, AssetPool::GetColor().AppWaveform.colorStateLabelRecording);

        // Time
        int total_seconds = (HAL::Millis() - _data.recording_start_time_count) / 1000;
        int minutes = total_seconds / 60;
        int seconds = total_seconds % 60;
        Waveform::_get_string_buffer() = spdlog::fmt_lib::format("REC: {:02d}:{:02d}", minutes, seconds);

        // Panel
        auto panel_width = HAL::GetCanvas()->textWidth(Waveform::_get_string_buffer().c_str());
        panel_width += 10;
        HAL::GetCanvas()->fillSmoothRoundRect(_state_label_mergin_left_recording,
                                              _state_label_mergin_top_recording,
                                              panel_width,
                                              23,
                                              6,
                                              AssetPool::GetColor().AppWaveform.colorStateLabelRecording);

        // Label
        HAL::GetCanvas()->drawString(Waveform::_get_string_buffer().c_str(),
                                     _state_label_mergin_left_recording + 5,
                                     _state_label_mergin_top_recording + 4);
    }
}

/* -------------------------------------------------------------------------- */
/*                                  Recording                                 */
/* -------------------------------------------------------------------------- */
void WaveFormRecorder::_handle_start_recording()
{
    // Free if exist
    if (_data.trigger != nullptr)
        delete _data.trigger;

    // Create new trigger and apply config
    switch (getConfig().trigger_mode)
    {
    case trigger_mode_manual:
    {
        spdlog::info("create trigger_mode_manual");
        _data.trigger = new Trigger_Manual;
        break;
    }
    case trigger_mode_v_above_level:
    {
        spdlog::info("create trigger_mode_v_above_level");
        _data.trigger = new Trigger_Level(true);
        _data.trigger->setTriggerChannel(true);
        break;
    }
    case trigger_mode_v_under_level:
    {
        spdlog::info("create trigger_mode_v_under_level");
        _data.trigger = new Trigger_Level(false);
        _data.trigger->setTriggerChannel(true);
        break;
    }
    case trigger_mode_a_above_level:
    {
        spdlog::info("create trigger_mode_a_above_level");
        _data.trigger = new Trigger_Level(true);
        _data.trigger->setTriggerChannel(false);
        break;
    }
    case trigger_mode_a_under_level:
    {
        spdlog::info("create trigger_mode_a_under_level");
        _data.trigger = new Trigger_Level(false);
        _data.trigger->setTriggerChannel(false);
        break;
    }
    default:
    {
        spdlog::error("no trigger mode {}", getConfig().trigger_mode);
        return;
    }
    }

    // Apply config
    _data.trigger->setRecordTime(getConfig().record_time);
    _data.trigger->setThreshold(getConfig().threshold);

    // Start recording
    HAL::DestroyVaRecorder();
    HAL::CreatVaRecorder(_data.trigger);
}
