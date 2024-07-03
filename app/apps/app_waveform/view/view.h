/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "../../../hal/hal.h"
#include "config_panel/config_panel.h"
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <string>

#define _ENABLE_BENCHMARK

namespace VIEWS
{
    /* -------------------------------------------------------------------------- */
    /*                            View basic wave form                            */
    /* -------------------------------------------------------------------------- */
    class Waveform
    {
    private:
        // Bg color
        SmoothUIToolKit::SmoothRGB _bg_color;
        lgfx::rgb888_t _bg_color_buffer;

    protected:
        // Chart
        struct ChartProps_t
        {
            SmoothUIToolKit::Chart::SmoothLineChart chart_v;
            SmoothUIToolKit::Chart::SmoothLineChart chart_a;

            // Buffer
            SmoothUIToolKit::Vector2D_t last_p;
            SmoothUIToolKit::Vector2D_t new_p;
            int p_x = 0;
            int temp_buffer = 0;
            bool stop_render = false;

            int current_x_range = 0;
            float current_v_y_range_top = -1;
            float current_v_y_range_bottom = 0;
            float current_a_y_range_top = -1;
            float current_a_y_range_bottom = 0;

            std::string string_buffer;

            bool is_x_range_changed = false;
            uint32_t x_range_changed_notice_count = 0;
        };
        ChartProps_t _chart_props;

    protected:
        // Panel
        struct PanelProps_t
        {
            // X as V's panel's y transition, Y as A's panel's y transition.
            SmoothUIToolKit::Transition2D panel_transition;
        };
        PanelProps_t _panel_props;

    protected:
        // Input
        struct InputProps_t
        {
            uint32_t pm_data_update_time_count = 0;

            SmoothUIToolKit::RingBuffer<float, 1> pm_data_buffer_v;
            SmoothUIToolKit::RingBuffer<float, 1> pm_data_buffer_a;

            float min_v = 114514;
            float max_v = -114514;
            float min_a = 114514;
            float max_a = -114514;

            float last_min_v = 114514;
            float last_max_v = -114514;
            float last_min_a = 114514;
            float last_max_a = -114514;

            bool is_paused = false;
            bool update_chart_x = true;

#ifdef _ENABLE_BENCHMARK
            uint32_t buffer_start_time_count = 0;
            bool bench_mark_done = false;
#endif
        };
        InputProps_t _input_props;

    protected:
        void _update_pm_data();
        const uint32_t& _get_pm_data_a_scale();
        void _update_chart_x_zoom();
        void _update_chart_y_zoom(bool applyChartZoom = true);
        void _update_chart_y_zoom_with_third_value(const float& thirdV, const float& thirdA);
        virtual void _update_input();
        void _update_transition();

        void _render_background();
        void _render_y_scales();
        void _render_x_scales_notice();
        virtual void _on_render_background_finish() {}
        void _render_wave();
        inline std::string& _get_string_buffer() { return _chart_props.string_buffer; }
        void _render_panels();
        void _update_render(bool pushBuffer = false, bool renderPanel = true, bool renderXScaleNotice = true);

    public:
        Waveform(uint32_t themeColor);
        virtual ~Waveform();
        virtual void update();
        inline void setPause(bool pause) { _input_props.is_paused = pause; }
        inline bool isPaused() { return _input_props.is_paused; }
        void enableChartXUpdate(bool enable);
    };

    /* -------------------------------------------------------------------------- */
    /*            Add recording status display, add record config panel           */
    /* -------------------------------------------------------------------------- */
    class WaveFormRecorder : public Waveform
    {
    private:
        enum State_t
        {
            state_idle = 0,
            state_waiting_trigger,
            state_recording,
            state_saving,
        };

        struct Data_t
        {
            State_t state = state_idle;
            bool has_finished_recording = false;
            bool want_to_quit = false;

            ConfigPanel* config_panel = nullptr;
            VA_RECORDER::TriggerBase* trigger = nullptr;

            float threshold_a_scaled_buffer = 0.0f;
            uint32_t recording_start_time_count = 0;

            int string_y_offset = 0;
        };
        Data_t _data;
        inline const ConfigPanel::Config_t& getConfig() { return _data.config_panel->getConfig(); }

        void _update_input() override;
        void _update_chart();
        void _update_state_idle();
        void _update_state_waiting_trigger();
        void _update_state_recording();
        void _update_state_saving();

        void _handle_render();
        void _render_mode_icon();
        void _render_threshold_line();
        void _render_rec_state_label();

        void _handle_start_recording();

    public:
        WaveFormRecorder(uint32_t themeColor) : Waveform(themeColor) {}
        ~WaveFormRecorder();

        void init();
        void update() override;
        inline bool hasFinishedRecording() { return _data.has_finished_recording; }
        inline bool want2quit() { return _data.want_to_quit; }
    };
} // namespace VIEWS
