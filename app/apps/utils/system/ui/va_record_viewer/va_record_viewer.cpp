/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "va_record_viewer.h"
#include "../../inputs/inputs.h"
#include "../../../../../assets/assets.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <mooncake.h>
#include <string>
#include <vector>

using namespace SYSTEM::UI;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

// Power monitor data
static constexpr uint32_t _pm_data_update_interval = 35;
static constexpr uint32_t _pm_data_a_scale = 1000;

// Chart
static constexpr int _guide_line_num = 12;
static constexpr int _guide_line_padding = 240 / _guide_line_num;

void VaRecordViewer::init()
{
    if (_data.record == nullptr)
        return;

    // Bg mask
    _data.bg_mask_anim.setDuration(600);
    _data.bg_mask_anim.jumpTo(120, 320);
    _data.bg_mask_anim.moveTo(120, 120);

    // Transition config
    _data.chart_v.setOrigin(0, 35 - 18);
    _data.chart_a.setOrigin(0, 35 - 18);
    _data.chart_v.setSize(240, 170);
    _data.chart_a.setSize(240, 170);

    _data.chart_v.getZoomTransition().setTransitionPath(EasingPath::easeOutBack);
    _data.chart_v.getZoomTransition().setDuration(400);
    _data.chart_v.getOffsetTransition().setTransitionPath(EasingPath::easeOutBack);
    _data.chart_v.getOffsetTransition().setDuration(400);

    _data.chart_a.getZoomTransition().setTransitionPath(EasingPath::easeOutBack);
    _data.chart_a.getZoomTransition().setDuration(400);
    _data.chart_a.getOffsetTransition().setTransitionPath(EasingPath::easeOutBack);
    _data.chart_a.getOffsetTransition().setDuration(400);

    // Reset
    _reset_chart();
    HAL::ResetEncoderCount();
}

void VaRecordViewer::update(uint32_t currentTime)
{
    // Input
    _update_input();

    // Transition
    _data.bg_mask_anim.update(currentTime);
    _data.chart_v.update(currentTime);
    _data.chart_a.update(currentTime);

    // Render
    _render_background();
    _render_wave();
    _render_panel();
    HAL::CanvasUpdate();
}

void VaRecordViewer::_get_record_data_max_min()
{
    if (_data.record == nullptr)
        return;

    _data.record_data_max.voltage = -233.3f;
    _data.record_data_max.current = -233.3f;
    _data.record_data_min.voltage = 9999.9f;
    _data.record_data_min.current = 9999.9f;

    for (auto& i : *_data.record)
    {
        // Apply scale
        if (!_data.data_a_is_scaled)
            i.current *= _pm_data_a_scale;

        if (i.voltage > _data.record_data_max.voltage)
            _data.record_data_max.voltage = i.voltage;
        else if (i.voltage < _data.record_data_min.voltage)
            _data.record_data_min.voltage = i.voltage;

        if (i.current > _data.record_data_max.current)
            _data.record_data_max.current = i.current;
        else if (i.current < _data.record_data_min.current)
            _data.record_data_min.current = i.current;
    }
    _data.data_a_is_scaled = true;

    spdlog::info("max: {:.2f} V {:.4f} A", _data.record_data_max.voltage, _data.record_data_max.current);
    spdlog::info("min: {:.2f} V {:.4f} A", _data.record_data_min.voltage, _data.record_data_min.current);
}

void VaRecordViewer::_reset_chart()
{
    // Default X range
    _data.chart_x_zoom = _data.record->size();
    if (_data.chart_x_zoom > 240)
        _data.chart_x_zoom = 240;

    if (_data.chart_x_zoom != 0)
    {
        _data.chart_v.moveXIntoRange(0, static_cast<float>(_data.chart_x_zoom));
        _data.chart_a.moveXIntoRange(0, static_cast<float>(_data.chart_x_zoom));
    }

    // Default Y range
    _get_record_data_max_min();
    _data.chart_v_zoom = _data.record_data_max.voltage;
    _data.chart_a_zoom = _data.record_data_max.current;

    if (_data.chart_v_zoom != 0)
        _data.chart_v.moveYIntoRange(0, _data.chart_v_zoom);
    if (_data.chart_a_zoom != 0)
        _data.chart_a.moveYIntoRange(0, _data.chart_a_zoom);

    _data.chart_x_scroll = 0;
    _data.chart_v_scroll = 0;
    _data.chart_a_scroll = 0;
}

void VaRecordViewer::_update_input()
{
    if (_data.record == nullptr)
        return;

    Button::Update();

    // Update state
    if (Button::Encoder()->wasClicked())
    {
        int new_state = static_cast<int>(_data.state);
        new_state++;
        if (new_state >= static_cast<int>(state_num))
            new_state = 0;
        _data.state = static_cast<State_t>(new_state);
    }

    _update_encoder();
    if (_data.encoder_direction != 0)
    {
        switch (_data.state)
        {
        case state_chart_x_scoll:
            _update_x_scroll();
            break;
        case state_chart_x_zoom:
            _update_x_zoom();
            break;
        case state_chart_v_scroll:
            _update_v_scroll();
            break;
        case state_chart_v_zoom:
            _update_v_zoom();
            break;
        case state_chart_a_scroll:
            _update_a_scroll();
            break;
        case state_chart_a_zoom:
            _update_a_zoom();
            break;
        default:
            break;
        }
    }

    // Reset
    if (Button::Side()->wasClicked())
        _reset_chart();

    // Quit
    else if (Button::Side()->wasHold())
        _data.want_to_quit = true;
}

void VaRecordViewer::_update_encoder()
{
    auto new_encoder_count = HAL::GetEncoderCount();

    if (new_encoder_count > _data.last_encoder_count)
        _data.encoder_direction = 1;
    else if (new_encoder_count < _data.last_encoder_count)
        _data.encoder_direction = -1;
    else
        _data.encoder_direction = 0;

    _data.last_encoder_count = new_encoder_count;
}

static constexpr int _chart_x_scoll_max = 100000;
static constexpr int _chart_x_scoll_min = -_chart_x_scoll_max;
static constexpr int _chart_x_scoll_step = 5;

/* -------------------------------------------------------------------------- */
/*                                 View update                                */
/* -------------------------------------------------------------------------- */
void VaRecordViewer::_update_x_scroll()
{
    if (_data.encoder_direction > 0)
        _data.chart_x_scroll += _chart_x_scoll_step;
    else
        _data.chart_x_scroll -= _chart_x_scoll_step;

    // Apply
    // spdlog::info("{}", _data.chart_x_scroll);
    _data.chart_v.moveXOffsetTo(static_cast<float>(_data.chart_x_scroll));
    _data.chart_a.moveXOffsetTo(static_cast<float>(_data.chart_x_scroll));
}

static constexpr int _chart_x_zoom_max = 720;
static constexpr int _chart_x_zoom_min = 30;
static constexpr int _chart_x_zoom_step = 30;

void VaRecordViewer::_update_x_zoom()
{
    if (_data.encoder_direction < 0)
    {
        _data.chart_x_zoom += _chart_x_zoom_step;
        if (_data.chart_x_zoom > _chart_x_zoom_max)
            _data.chart_x_zoom = _chart_x_zoom_max;
    }
    else
    {
        _data.chart_x_zoom -= _chart_x_zoom_step;
        if (_data.chart_x_zoom < _chart_x_zoom_min)
            _data.chart_x_zoom = _chart_x_zoom_min;
    }

    // spdlog::info("{}", _data.chart_x_zoom);

    // Apply
    _data.chart_v.moveXIntoRange(0, static_cast<float>(_data.chart_x_zoom));
    _data.chart_a.moveXIntoRange(0, static_cast<float>(_data.chart_x_zoom));
}

// static constexpr float _chart_v_scroll_max = 666;
// static constexpr float _chart_v_scroll_min = -666;
static constexpr float _chart_v_scroll_step = 0.1f;

void VaRecordViewer::_update_v_scroll()
{
    if (_data.encoder_direction > 0)
    {
        _data.chart_v_scroll += _chart_v_scroll_step;
        // if (_data.chart_v_scroll > _chart_v_scroll_max)
        //     _data.chart_v_scroll = _chart_v_scroll_max;
    }
    else
    {
        _data.chart_v_scroll -= _chart_v_scroll_step;
        // if (_data.chart_v_scroll < _chart_v_scroll_min)
        //     _data.chart_v_scroll = _chart_v_scroll_min;
    }

    // spdlog::info("{}", _data.chart_v_scroll);

    // Apply
    _data.chart_v.moveYOffsetTo(_data.chart_v_scroll);
}

static constexpr float _chart_v_zoom_max = 12.0f;
static constexpr float _chart_v_zoom_min = 1.0f;
static constexpr float _chart_v_zoom_step = 0.5f;

void VaRecordViewer::_update_v_zoom()
{
    if (_data.encoder_direction < 0)
    {
        _data.chart_v_zoom += _chart_v_zoom_step;
        if (_data.chart_v_zoom > _chart_v_zoom_max)
            _data.chart_v_zoom = _chart_v_zoom_max;
    }
    else
    {
        _data.chart_v_zoom -= _chart_v_zoom_step;
        if (_data.chart_v_zoom < _chart_v_zoom_min)
            _data.chart_v_zoom = _chart_v_zoom_min;
    }

    // spdlog::info("{}", _data.chart_v_zoom);

    // Apply
    _data.chart_v.moveYIntoRange(0, _data.chart_v_zoom);
}

// static constexpr float _chart_a_scroll_max = 666;
// static constexpr float _chart_a_scroll_min = -666;
static constexpr float _chart_a_scroll_step = 0.01f * _pm_data_a_scale;

void VaRecordViewer::_update_a_scroll()
{
    if (_data.encoder_direction > 0)
    {
        _data.chart_a_scroll += _chart_a_scroll_step;
        // if (_data.chart_a_scroll > _chart_a_scroll_max)
        //     _data.chart_a_scroll = _chart_a_scroll_max;
    }
    else
    {
        _data.chart_a_scroll -= _chart_a_scroll_step;
        // if (_data.chart_a_scroll < _chart_a_scroll_min)
        //     _data.chart_a_scroll = _chart_a_scroll_min;
    }

    // spdlog::info("{}", _data.chart_a_scroll);

    // Apply
    _data.chart_a.moveYOffsetTo(_data.chart_a_scroll);
}

static constexpr float _chart_a_zoom_max = 1.0f * _pm_data_a_scale;
static constexpr float _chart_a_zoom_min = 0.1f * _pm_data_a_scale;
static constexpr float _chart_a_zoom_step = 0.02f * _pm_data_a_scale;

void VaRecordViewer::_update_a_zoom()
{
    if (_data.encoder_direction < 0)
    {
        _data.chart_a_zoom += _chart_a_zoom_step;
        if (_data.chart_a_zoom > _chart_a_zoom_max)
            _data.chart_a_zoom = _chart_a_zoom_max;
    }
    else
    {
        _data.chart_a_zoom -= _chart_a_zoom_step;
        if (_data.chart_a_zoom < _chart_a_zoom_min)
            _data.chart_a_zoom = _chart_a_zoom_min;
    }

    // spdlog::info("{}", _data.chart_a_zoom);

    // Apply
    _data.chart_a.moveYIntoRange(0, _data.chart_a_zoom);
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
static const uint32_t _color_chart_bg = 0xFFFFFF;

void VaRecordViewer::_render_background()
{
    // Background
    if (_data.bg_mask_anim.isFinish())
    {
        HAL::GetCanvas()->fillScreen(_color_chart_bg);
    }
    else
    {
        HAL::GetCanvas()->fillScreen(AssetPool::GetStaticAsset()->Color.AppFile.selector);
        HAL::GetCanvas()->fillSmoothCircle(
            _data.bg_mask_anim.getValue().x, _data.bg_mask_anim.getValue().y, 200, _color_chart_bg);
    }

    // Guide line
    for (int i = 0; i < _guide_line_num; i++)
    {
        HAL::GetCanvas()->drawFastVLine(i * _guide_line_padding,
                                        0 + _data.bg_mask_anim.getValue().y - 120,
                                        240,
                                        AssetPool::GetColor().AppWaveform.colorGuideLine);
        HAL::GetCanvas()->drawFastHLine(0,
                                        i * _guide_line_padding + _data.bg_mask_anim.getValue().y - 120,
                                        240,
                                        AssetPool::GetColor().AppWaveform.colorGuideLine);
    }
}

void VaRecordViewer::_render_wave()
{
    if (_data.record == nullptr)
        return;

    Vector2D_t last_p_v;
    Vector2D_t last_p_a;
    Vector2D_t new_p_v;
    Vector2D_t new_p_a;
    int chart_x = 0;

    for (const auto& i : *_data.record)
    {
        // V
        new_p_v = _data.chart_v.getChartPoint(chart_x, i.voltage);
        if (chart_x != 0)
        {
            // Fake width line
            for (int i = 0; i < 3; i++)
            {
                HAL::GetCanvas()->drawLine(
                    last_p_v.x + i, last_p_v.y, new_p_v.x + i, new_p_v.y, AssetPool::GetColor().AppWaveform.colorLineV);
                HAL::GetCanvas()->drawLine(
                    last_p_v.x, last_p_v.y + i, new_p_v.x, new_p_v.y + i, AssetPool::GetColor().AppWaveform.colorLineV);
            }
        }

        // A
        new_p_a = _data.chart_a.getChartPoint(chart_x, i.current);
        if (chart_x != 0)
        {
            // Fake width line
            for (int i = 0; i < 3; i++)
            {
                HAL::GetCanvas()->drawLine(
                    last_p_a.x + i, last_p_a.y, new_p_a.x + i, new_p_a.y, AssetPool::GetColor().AppWaveform.colorLineA);
                HAL::GetCanvas()->drawLine(
                    last_p_a.x, last_p_a.y + i, new_p_a.x, new_p_a.y + i, AssetPool::GetColor().AppWaveform.colorLineA);
            }
        }

        last_p_v = new_p_v;
        last_p_a = new_p_a;
        chart_x++;
    }
}

static const std::vector<std::string> _state_Label_text_list = {
    "X  Zoom", "X  Scroll", "Vol.  Zoom", "Vol.  Scroll", "Cur.  Zoom", "Cur.  Scroll"};

void VaRecordViewer::_render_panel()
{
    HAL::GetCanvas()->fillRect(
        0, 240 - 29 + _data.bg_mask_anim.getValue().y - 120, 240, 29, AssetPool::GetStaticAsset()->Color.AppFile.selector);

    HAL::GetCanvas()->setTextDatum(top_center);
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppFile.optionText,
                                   AssetPool::GetStaticAsset()->Color.AppFile.selector);
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
    HAL::GetCanvas()->drawString(_state_Label_text_list[static_cast<size_t>(_data.state)].c_str(),
                                 120,
                                 240 - 29 + _data.bg_mask_anim.getValue().y - 120);
}

void VaRecordViewer::CreateAndWait(std::vector<VA_RECORDER::RecordData_t>* record)
{
    spdlog::info("create va record viewer");

    if (record->size() == 0)
    {
        HAL::PopWarning("Record is empty");
        return;
    }

    auto viewer = new VaRecordViewer;

    viewer->setRecord(record);
    viewer->init();

    while (1)
    {
        HAL::FeedTheDog();
        viewer->update(HAL::Millis());
        if (viewer->want2quit())
            break;
    }

    delete viewer;
}
