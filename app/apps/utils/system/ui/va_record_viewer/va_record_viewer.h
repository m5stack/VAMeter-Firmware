/**
 * @file va_record_viewer.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "../../../../../hal/hal.h"
#include <cstdint>
#include <smooth_ui_toolkit.h>
#include <string>
#include <vector>

namespace SYSTEM
{
    namespace UI
    {
        /**
         * @brief A chart viewer with controls.
         *
         */
        class VaRecordViewer
        {
        private:
            enum State_t
            {
                state_chart_x_zoom = 0,
                state_chart_x_scoll,
                state_chart_v_zoom,
                state_chart_v_scroll,
                state_chart_a_zoom,
                state_chart_a_scroll,
                state_num,
            };

            struct Data_t
            {
                SmoothUIToolKit::Transition2D bg_mask_anim;

                SmoothUIToolKit::Chart::SmoothLineChart chart_v;
                SmoothUIToolKit::Chart::SmoothLineChart chart_a;
                VA_RECORDER::RecordData_t record_data_max;
                VA_RECORDER::RecordData_t record_data_min;

                std::vector<VA_RECORDER::RecordData_t>* record = nullptr;

                bool want_to_quit = false;
                bool data_a_is_scaled = false;

                State_t state = state_chart_x_zoom;

                int last_encoder_count = 0;
                int encoder_direction = 0;

                int chart_x_zoom = 240;
                int chart_x_scroll = 0;
                float chart_v_scroll = 0.0f;
                float chart_v_zoom = 0.0f;
                float chart_a_scroll = 0.0f;
                float chart_a_zoom = 0.0f;
            };
            Data_t _data;
            void _get_record_data_max_min();
            void _reset_chart();

            void _update_input();
            void _update_encoder();
            void _update_x_scroll();
            void _update_x_zoom();
            void _update_v_scroll();
            void _update_v_zoom();
            void _update_a_scroll();
            void _update_a_zoom();

            void _render_background();
            void _render_wave();
            void _render_panel();

        public:
            inline void setRecord(std::vector<VA_RECORDER::RecordData_t>* record) { _data.record = record; }
            inline bool want2quit() { return _data.want_to_quit; }

            void init();
            void update(uint32_t currentTime);

        public:
            static void CreateAndWait(std::vector<VA_RECORDER::RecordData_t>* record);
        };
    } // namespace UI
} // namespace SYSTEM
