/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "view.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include <cstdint>
#include <mooncake.h>
#include <smooth_ui_toolkit.h>
#include <string>
#include <vector>

using namespace SmoothUIToolKit;
using namespace SmoothUIToolKit::Chart;
using namespace SYSTEM::INPUTS;
using namespace VIEWS;

// TODO
// spline instead of draw line

#define _ENABLE_BENCHMARK

/* -------------------------------------------------------------------------- */
/*                                    View                                    */
/* -------------------------------------------------------------------------- */
// Power monitor data
static constexpr uint32_t _pm_data_update_interval = 35;
static constexpr uint32_t _pm_data_buffer_len = 240;
static constexpr uint32_t _pm_data_buffer_max_time = _pm_data_update_interval * _pm_data_buffer_len;

// Scale A value up, since smooth chart has only .3f presition
static constexpr uint32_t _pm_data_a_scale = 1000;
// static constexpr uint32_t _pm_data_a_scale = 1;

// Chart
static constexpr int _guide_line_num = 12;
static constexpr int _guide_line_padding = 240 / _guide_line_num;
// static constexpr uint32_t _guide_line_color = 0xD4D4D4;
// static constexpr uint32_t _line_v_color = 0x3D7AF5;
// static constexpr uint32_t _line_a_color = 0xF83434;
// static constexpr uint32_t _line_v_color = 0x0000FF;
// static constexpr uint32_t _line_a_color = 0xFF0000;

// Chart range
static constexpr int _chart_max_x_range = 240;
static constexpr int _chart_min_x_range = 30;
static constexpr int _chart_x_range_step = 30;
static constexpr float _chart_v_min_y_range = 2;
static constexpr float _chart_a_min_y_range_big = 0.01 * _pm_data_a_scale;
static constexpr float _chart_a_min_y_range_small = 0.001 * _pm_data_a_scale;

/* -------------------------------------------------------------------------- */
/*                                    Setup                                   */
/* -------------------------------------------------------------------------- */
Waveform::Waveform(uint32_t themeColor)
{
    // Background
    _bg_color.jumpTo(Hex2Rgb(themeColor));
    _bg_color.moveTo(255, 255, 255);
    _bg_color.setDuration(400);

    // Panel
    _panel_props.panel_transition.jumpTo(-27, 240);
    _panel_props.panel_transition.moveTo(0, 213);
    _panel_props.panel_transition.setDuration(400);
    // _panel_props.panel_transition.setTransitionPath(EasingPath::easeOutBack);

    // Chart
    _chart_props.chart_v.setOrigin(0, 35);
    _chart_props.chart_a.setOrigin(0, 35);
    _chart_props.chart_v.setSize(240, 170);
    _chart_props.chart_a.setSize(240, 170);

    _chart_props.chart_v.getZoomTransition().getXTransition().setTransitionPath(EasingPath::easeOutBack);
    _chart_props.chart_v.getZoomTransition().getXTransition().setDuration(400);
    _chart_props.chart_v.getZoomTransition().getYTransition().setDuration(400);
    _chart_props.chart_v.getOffsetTransition().setDuration(400);

    _chart_props.chart_a.getZoomTransition().getXTransition().setTransitionPath(EasingPath::easeOutBack);
    _chart_props.chart_a.getZoomTransition().getXTransition().setDuration(400);
    _chart_props.chart_a.getZoomTransition().getYTransition().setDuration(400);
    _chart_props.chart_a.getOffsetTransition().setDuration(400);

    // _chart_props.chart_v.moveYIntoRange(0, 8);
    // _chart_props.chart_a.moveYIntoRange(0, 0.5 * _pm_data_a_scale);

    // _chart_props.chart_v.moveXIntoRange(0, 120);
    // _chart_props.chart_a.moveXIntoRange(0, 120);

    // Reset
    HAL::ResetEncoderCount();
    HAL::GetCanvas()->setTextSize(1);
    _chart_props.current_x_range = _chart_max_x_range;

    // Buffer
    _input_props.pm_data_buffer_v.reSize(_pm_data_buffer_len);
    _input_props.pm_data_buffer_a.reSize(_pm_data_buffer_len);
}

Waveform::~Waveform() {}

/* -------------------------------------------------------------------------- */
/*                                    Input                                   */
/* -------------------------------------------------------------------------- */
void Waveform::_update_pm_data()
{
    if (_input_props.is_paused)
        return;

    // Update power monitor
    if (HAL::Millis() - _input_props.pm_data_update_time_count > _pm_data_update_interval)
    {
        HAL::UpdatePowerMonitor();
        _input_props.pm_data_buffer_v.put(HAL::GetPowerMonitorData().busVoltage);
        _input_props.pm_data_buffer_a.put(HAL::GetPowerMonitorData().shuntCurrent * _pm_data_a_scale);

        _input_props.pm_data_update_time_count = HAL::Millis();

#ifdef _ENABLE_BENCHMARK
        if (!_input_props.bench_mark_done)
        {
            if (_input_props.pm_data_buffer_v.valueNum() == 1)
            {
                spdlog::info("start..");
                _input_props.buffer_start_time_count = HAL::Millis();
            }
            else if (_input_props.pm_data_buffer_v.isFull())
            {
                _input_props.bench_mark_done = true;
                spdlog::info("buffer len: {}", _pm_data_buffer_len);
                spdlog::info("ideal full buffer time: {} ms", _pm_data_buffer_max_time);
                auto takes_time = HAL::Millis() - _input_props.buffer_start_time_count;
                spdlog::info("actual full buffer time: {} ms", takes_time);
                spdlog::info("ideal put interval: {} ms", _pm_data_update_interval);
                spdlog::info("actual put interval: {} ms", (float)takes_time / _pm_data_buffer_len);
            }
        }
#endif
    }
}

const uint32_t& Waveform::_get_pm_data_a_scale() { return _pm_data_a_scale; }

void Waveform::enableChartXUpdate(bool enable)
{
    _input_props.update_chart_x = enable;
    if (enable)
        Encoder::Reset();
}

void Waveform::_update_chart_x_zoom()
{
    if (!_input_props.update_chart_x)
        return;

    // Encoder::Update();
    if (Encoder::WasMoved())
    {
        if (Encoder::GetDirection() > 0)
            _chart_props.current_x_range += _chart_x_range_step;
        else
            _chart_props.current_x_range -= _chart_x_range_step;

        if (_chart_props.current_x_range > _chart_max_x_range)
            _chart_props.current_x_range = _chart_max_x_range;
        else if (_chart_props.current_x_range < _chart_min_x_range)
            _chart_props.current_x_range = _chart_min_x_range;

        // Update chart
        _chart_props.chart_v.moveXIntoRange(0, static_cast<float>(_chart_props.current_x_range));
        _chart_props.chart_a.moveXIntoRange(0, static_cast<float>(_chart_props.current_x_range));

        // Notice panel
        _chart_props.is_x_range_changed = true;
        _chart_props.x_range_changed_notice_count = HAL::Millis();
    }
}

static uint8_t _first_few_update_count = 0;

void Waveform::_update_chart_y_zoom(bool applyChartZoom)
{
    // If first time
    if (_input_props.min_v == 114514 || _input_props.min_a == 114514)
    {
        return;
    }

    // V chart y range
    if (_input_props.min_v != _input_props.last_min_v || _input_props.max_v != _input_props.last_max_v)
    {
        // Top
        _chart_props.current_v_y_range_top = _input_props.max_v;

        // Bottom
        _chart_props.current_v_y_range_bottom = _input_props.min_v;

        // Min range limit
        if (_chart_props.current_v_y_range_top - _chart_props.current_v_y_range_bottom < _chart_v_min_y_range)
        {
            auto mid_point = (_input_props.max_v - _input_props.min_v) / 2 + _input_props.min_v;
            _chart_props.current_v_y_range_top = mid_point + _chart_v_min_y_range / 2;
            _chart_props.current_v_y_range_bottom = _chart_props.current_v_y_range_top - _chart_v_min_y_range;
        }

        // Update range
        // spdlog::info("v [{:.2f} {:.2f}] [{:.2f} {:.2f}]",
        //              _input_props.min_v,
        //              _input_props.max_v,
        //              _chart_props.current_v_y_range_bottom,
        //              _chart_props.current_v_y_range_top);
        if (applyChartZoom)
            _chart_props.chart_v.moveYIntoRange(_chart_props.current_v_y_range_bottom, _chart_props.current_v_y_range_top);

        _input_props.last_min_v = _input_props.min_v;
        _input_props.last_max_v = _input_props.max_v;
    }

    // A chart y range
    if (_input_props.min_a != _input_props.last_min_a || _input_props.max_a != _input_props.last_max_a)
    {
        // Top
        _chart_props.current_a_y_range_top = _input_props.max_a;

        // Bottom
        _chart_props.current_a_y_range_bottom = _input_props.min_a;

        // Min range limit
        if (_chart_props.current_a_y_range_top - _chart_props.current_a_y_range_bottom < _chart_a_min_y_range_small)
        {
            auto mid_point = (_input_props.max_a - _input_props.min_a) / 2 + _input_props.min_a;
            _chart_props.current_a_y_range_top = mid_point + _chart_a_min_y_range_small / 2;
            _chart_props.current_a_y_range_bottom = _chart_props.current_a_y_range_top - _chart_a_min_y_range_small;
        }
        else if (_chart_props.current_a_y_range_top - _chart_props.current_a_y_range_bottom < _chart_a_min_y_range_big)
        {
            auto mid_point = (_input_props.max_a - _input_props.min_a) / 2 + _input_props.min_a;
            _chart_props.current_a_y_range_top = mid_point + _chart_a_min_y_range_big / 2;
            _chart_props.current_a_y_range_bottom = _chart_props.current_a_y_range_top - _chart_a_min_y_range_big;
        }

        // // Update range
        // spdlog::info("a [{:.2f} {:.2f}] [{:.2f} {:.2f}]",
        //              _input_props.min_a,
        //              _input_props.max_a,
        //              _chart_props.current_a_y_range_bottom,
        //              _chart_props.current_a_y_range_top);
        if (applyChartZoom)
            _chart_props.chart_a.moveYIntoRange(_chart_props.current_a_y_range_bottom, _chart_props.current_a_y_range_top);

        _input_props.last_min_a = _input_props.min_a;
        _input_props.last_max_a = _input_props.max_a;
    }
}

// For recorder view usage, easer to handle here
void Waveform::_update_chart_y_zoom_with_third_value(const float& thirdV, const float& thirdA)
{
    // Normale range
    _update_chart_y_zoom(false);

    // V
    if (thirdV < _chart_props.current_v_y_range_bottom)
        _chart_props.current_v_y_range_bottom = thirdV;
    else if (thirdV > _chart_props.current_v_y_range_top)
        _chart_props.current_v_y_range_top = thirdV;
    // spdlog::info("v {} {}", _chart_props.current_v_y_range_bottom, _chart_props.current_v_y_range_top);
    if (_chart_props.current_v_y_range_bottom != _chart_props.current_v_y_range_top)
        _chart_props.chart_v.moveYIntoRange(_chart_props.current_v_y_range_bottom, _chart_props.current_v_y_range_top);

    // A
    if (thirdA < _chart_props.current_a_y_range_bottom)
        _chart_props.current_a_y_range_bottom = thirdA;
    else if (thirdA > _chart_props.current_a_y_range_top)
        _chart_props.current_a_y_range_top = thirdA;
    // spdlog::info("a {} {}", _chart_props.current_v_y_range_bottom, _chart_props.current_v_y_range_top);
    if (_chart_props.current_a_y_range_bottom != _chart_props.current_a_y_range_top)
        _chart_props.chart_a.moveYIntoRange(_chart_props.current_a_y_range_bottom, _chart_props.current_a_y_range_top);
}

void Waveform::_update_input()
{
    _update_pm_data();
    _update_chart_x_zoom();
    _update_chart_y_zoom();
}

/* -------------------------------------------------------------------------- */
/*                                 Transitions                                */
/* -------------------------------------------------------------------------- */
void Waveform::_update_transition()
{
    _bg_color.update(HAL::Millis());
    _panel_props.panel_transition.update(HAL::Millis());
    _chart_props.chart_v.update(HAL::Millis());
    _chart_props.chart_a.update(HAL::Millis());
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void Waveform::_render_background()
{
    // Background color
    if (_bg_color.isFinish())
        HAL::GetCanvas()->fillScreen(TFT_WHITE);
    else
    {
        _bg_color_buffer.r = _bg_color.getCurrentRed();
        _bg_color_buffer.g = _bg_color.getCurrentGreen();
        _bg_color_buffer.b = _bg_color.getCurrentBlue();
        HAL::GetCanvas()->fillScreen(_bg_color_buffer);
    }

    // Draw pause icon
    if (_input_props.is_paused)
    {
        HAL::GetCanvas()->fillSmoothCircle(26, 60, 14, (uint32_t)0xCECECE);
        HAL::GetCanvas()->fillSmoothRoundRect(19, 54, 5, 12, 2, TFT_WHITE);
        HAL::GetCanvas()->fillSmoothRoundRect(28, 54, 5, 12, 2, TFT_WHITE);
    }

    // Guide line
    for (int i = 0; i < _guide_line_num; i++)
    {
        _chart_props.temp_buffer = 240 - _chart_props.chart_v.getChartPoint(static_cast<float>(i * _guide_line_padding), 0).x;
        // spdlog::info("{}", _chart_props.temp_buffer);
        HAL::GetCanvas()->drawFastVLine(_chart_props.temp_buffer, 0, 240, AssetPool::GetColor().AppWaveform.colorGuideLine);

        _chart_props.temp_buffer = i * _guide_line_padding;
        HAL::GetCanvas()->drawFastHLine(0, _chart_props.temp_buffer, 240, AssetPool::GetColor().AppWaveform.colorGuideLine);
    }

    _render_y_scales();
    _on_render_background_finish();
}

static constexpr uint32_t _color_scale_label_v = 0xA4A4A4;
static constexpr uint32_t _color_scale_label_a = 0xA4A4A4;

void Waveform::_render_y_scales()
{
    int chart_y_buffer = 0;
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);

    /* -------------------------------- V scales -------------------------------- */
    {
        HAL::GetCanvas()->setTextColor(_color_scale_label_v);
        HAL::GetCanvas()->setTextDatum(middle_left);

        std::vector<float> v_scale_list;
        v_scale_list.resize(3);
        v_scale_list[0] = _chart_props.current_v_y_range_top;
        v_scale_list[1] = (_chart_props.current_v_y_range_top + _chart_props.current_v_y_range_bottom) / 2;
        v_scale_list[2] = _chart_props.current_v_y_range_bottom;

        for (const auto& i : v_scale_list)
        {
            chart_y_buffer = Waveform::_chart_props.chart_v.getChartPoint(0, i).y;
            _get_string_buffer() = fmt::format("{:.1f}{}", i, i == 0 ? "" : "V");
            HAL::GetCanvas()->drawString(_get_string_buffer().c_str(), 2, chart_y_buffer);
        }
    }

    /* -------------------------------- A scales -------------------------------- */
    HAL::GetCanvas()->setTextColor(_color_scale_label_a);
    HAL::GetCanvas()->setTextDatum(middle_right);

    std::string unit;
    std::vector<float> a_scale_list;
    a_scale_list.resize(3);
    // spdlog::info("{} {}", _chart_props.current_a_y_range_top, _chart_props.current_a_y_range_bottom);
    a_scale_list[0] = _chart_props.current_a_y_range_top;
    a_scale_list[1] = (_chart_props.current_a_y_range_top + _chart_props.current_a_y_range_bottom) / 2;
    a_scale_list[2] = _chart_props.current_a_y_range_bottom;

    HAL::GetCanvas()->setTextColor(_color_scale_label_a);
    HAL::GetCanvas()->setTextDatum(middle_right);
    for (auto& i : a_scale_list)
    {
        // Get chart y
        chart_y_buffer = Waveform::_chart_props.chart_a.getChartPoint(0, i).y;

        // Scale unit
        if (std::abs(i) < 1)
        {
            i *= 1000;
            unit = "μA";
        }
        else if (std::abs(i) < 1000)
            unit = "mA";
        else
        {
            i /= 1000;
            unit = "A";
        }

        // Round
        i = std::round(i * 10) / 10;
        if (i == 0)
            unit = "";

        _get_string_buffer() = fmt::format("{}{}", i, unit);
        HAL::GetCanvas()->drawString(_get_string_buffer().c_str(), 237, chart_y_buffer);
    }
}

static const uint32_t _color_panel_x_scale_notice = 0x9B9B9B;
static const uint32_t _color_panel_x_scale_notice_text = 0xFFFFFF;
static constexpr int _panel_x_scale_notice_width = 240;
static constexpr int _panel_x_scale_notice_height = 40;
static constexpr int _panel_x_scale_notice_ml = 0;
static constexpr int _panel_x_scale_notice_mt = 100;
static constexpr int _panel_x_scale_notice_value_a_ml = 19;
static constexpr int _panel_x_scale_notice_value_b_ml = 221;

void Waveform::_render_x_scales_notice()
{
    if (!_chart_props.is_x_range_changed)
        return;

    if (HAL::Millis() - _chart_props.x_range_changed_notice_count > 1000)
    {
        _chart_props.is_x_range_changed = false;
        return;
    }

    HAL::GetCanvas()->fillRectAlpha(0, 100, 240, 40, 160, TFT_BLACK);

    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->setTextColor(_color_panel_x_scale_notice_text);
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_24);

    auto time_range = _chart_props.current_x_range * 50 / 1000;
    _get_string_buffer() = fmt::format("{}s", time_range);
    HAL::GetCanvas()->drawString(_get_string_buffer().c_str(), 120, 120);

    HAL::GetCanvas()->drawString("<", _panel_x_scale_notice_ml + _panel_x_scale_notice_value_a_ml, 120);
    HAL::GetCanvas()->drawString(">", _panel_x_scale_notice_ml + _panel_x_scale_notice_value_b_ml, 120);
}

void Waveform::_render_wave()
{
    // An offset to align wave to right
    _chart_props.temp_buffer = _chart_props.chart_v.getChartPoint(_input_props.pm_data_buffer_v.valueNum(), 0).x - 240;
    // spdlog::info("{}", _chart_props.wave_x_offset);

    // Bus voltage
    _chart_props.p_x = 0;
    _chart_props.last_p.reset();
    _chart_props.stop_render = false;
    _input_props.max_v = 0;
    _input_props.min_v = 114514;
    _input_props.pm_data_buffer_v.peekAll([&](const float& value) {
        // Pass if out of range
        if (_chart_props.stop_render)
            return;
        if (_chart_props.last_p.x > 240)
        {
            _chart_props.stop_render = true;
            return;
        }

        // Get chart point
        _chart_props.new_p = _chart_props.chart_v.getChartPoint(static_cast<float>(_chart_props.p_x), value);
        _chart_props.new_p.x -= _chart_props.temp_buffer;

        // Offset to avoid overlap with a wave
        _chart_props.new_p.y -= 3;

        // Render
        if (_chart_props.p_x != 0 && _chart_props.last_p.x <= 240)
        {
            // Fake width
            for (int i = 0; i < 3; i++)
            {
                HAL::GetCanvas()->drawLine(_chart_props.last_p.x + i,
                                           _chart_props.last_p.y,
                                           _chart_props.new_p.x + i,
                                           _chart_props.new_p.y,
                                           AssetPool::GetColor().AppWaveform.colorLineV);
                HAL::GetCanvas()->drawLine(_chart_props.last_p.x,
                                           _chart_props.last_p.y + i,
                                           _chart_props.new_p.x,
                                           _chart_props.new_p.y + i,
                                           AssetPool::GetColor().AppWaveform.colorLineV);
            }
        }
        _chart_props.last_p = _chart_props.new_p;
        _chart_props.p_x++;
        // spdlog::info("{} {} {}", chart_x, current_p.x, current_p.y);

        // Update max and min from the buffer
        if (value > _input_props.max_v)
            _input_props.max_v = value;
        else if (value < _input_props.min_v)
            _input_props.min_v = value;
    });

    // Shunt current
    _chart_props.p_x = 0;
    _chart_props.last_p.reset();
    _chart_props.stop_render = false;
    _input_props.max_a = -114514;
    _input_props.min_a = 114514;
    _input_props.pm_data_buffer_a.peekAll([&](const float& value) {
        // Pass if out of range
        if (_chart_props.stop_render)
            return;
        if (_chart_props.last_p.x > 240)
        {
            _chart_props.stop_render = true;
            return;
        }

        // Get chart point
        _chart_props.new_p = _chart_props.chart_a.getChartPoint(static_cast<float>(_chart_props.p_x), value);
        _chart_props.new_p.x -= _chart_props.temp_buffer;

        // Render
        if (_chart_props.p_x != 0 && _chart_props.last_p.x <= 240)
        {
            // Fake width
            for (int i = 0; i < 3; i++)
            {
                HAL::GetCanvas()->drawLine(_chart_props.last_p.x + i,
                                           _chart_props.last_p.y,
                                           _chart_props.new_p.x + i,
                                           _chart_props.new_p.y,
                                           AssetPool::GetColor().AppWaveform.colorLineA);
                HAL::GetCanvas()->drawLine(_chart_props.last_p.x,
                                           _chart_props.last_p.y + i,
                                           _chart_props.new_p.x,
                                           _chart_props.new_p.y + i,
                                           AssetPool::GetColor().AppWaveform.colorLineA);
            }
        }
        _chart_props.last_p = _chart_props.new_p;
        _chart_props.p_x++;
        // spdlog::info("{} {} {}", chart_x, current_p.x, current_p.y);

        // Update max and min from the buffer
        if (value > _input_props.max_a)
            _input_props.max_a = value;
        else if (value < _input_props.min_a)
            _input_props.min_a = value;
    });
}

void Waveform::_render_panels()
{
    // Panel
    HAL::GetCanvas()->pushImage(
        0, _panel_props.panel_transition.getValue().x, 240, 27, AssetPool::GetStaticAsset()->Image.AppWaveform.voltage_panel);
    HAL::GetCanvas()->pushImage(
        0, _panel_props.panel_transition.getValue().y, 240, 27, AssetPool::GetStaticAsset()->Image.AppWaveform.current_panel);

    // Hero label
    HAL::GetCanvas()->setTextColor(TFT_WHITE);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);

    auto value_data_buffer = HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().busVoltage);
    _chart_props.string_buffer = spdlog::fmt_lib::format("{}{}", value_data_buffer.value, value_data_buffer.unit);

    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 120, _panel_props.panel_transition.getValue().x + 14);

    value_data_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().shuntCurrent);
    _chart_props.string_buffer = spdlog::fmt_lib::format("{}{}", value_data_buffer.value, value_data_buffer.unit);

    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 120, _panel_props.panel_transition.getValue().y + 15);

    // Max and min label
    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);

    // V max
    HAL::GetCanvas()->setTextDatum(middle_left);
    HAL::GetCanvas()->setTextColor((uint32_t)0xD8E4FD);

    _chart_props.string_buffer = HAL::GetUnitAdaptatedVoltage(_input_props.max_v).value;
    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 13, _panel_props.panel_transition.getValue().x + 14);

    // V min
    _chart_props.string_buffer = HAL::GetUnitAdaptatedVoltage(_input_props.min_v).value;
    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 13, _panel_props.panel_transition.getValue().y + 14);

    // A max
    HAL::GetCanvas()->setTextDatum(middle_right);
    HAL::GetCanvas()->setTextColor((uint32_t)0xFED6D6);

    _chart_props.string_buffer = HAL::GetUnitAdaptatedVoltage(_input_props.max_a / _pm_data_a_scale).value;
    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 228, _panel_props.panel_transition.getValue().x + 14);

    // A min
    _chart_props.string_buffer = HAL::GetUnitAdaptatedVoltage(_input_props.min_a / _pm_data_a_scale).value;
    HAL::GetCanvas()->drawString(_chart_props.string_buffer.c_str(), 228, _panel_props.panel_transition.getValue().y + 14);
}

void Waveform::_update_render(bool pushBuffer, bool renderPanel, bool renderXScaleNotice)
{
    _render_background();
    _render_wave();

    if (renderPanel)
        _render_panels();

    if (renderXScaleNotice)
        _render_x_scales_notice();

#ifdef ESP_PLATFORM
        // HAL::RenderFpsPanel();
#endif
    if (pushBuffer)
        HAL::CanvasUpdate();
}

/* -------------------------------------------------------------------------- */
/*                                   Update                                   */
/* -------------------------------------------------------------------------- */
void Waveform::update()
{
    _update_input();
    _update_transition();
    _update_render();
}
