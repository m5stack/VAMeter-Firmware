/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
#include <functional>
#include <smooth_ui_toolkit.h>
#include <string>
#include "../../../hal/hal.h"

namespace VIEWS
{
    class PmDataPage
    {
    public:
        struct Config_t
        {
            std::function<void(std::string& text)> getValueCallback = nullptr;
            std::function<void(std::string& text)> getLabelCallback = nullptr;
            void* panelImage = nullptr;
            uint32_t themeColor = 0;
            uint32_t updateInterval = 20;
            bool showLcModeLabel = false;
        };

    private:
        struct Data_t
        {
            SmoothUIToolKit::Transition2D value_postion;
            SmoothUIToolKit::Transition2D label_postion;
            SmoothUIToolKit::Transition2D panel_postion;
            SmoothUIToolKit::Transition2D mode_postion;
            SmoothUIToolKit::SmoothRGB bg_color;

            bool want_to_quit = false;
            bool is_just_create = true;
            bool is_lc_mode = false;
            std::string string_buffer;
            uint32_t time_count = 0;
            LGFX_Sprite* spinner_icon = nullptr;
            int spinner_angle = 0;

            // Special pages
            bool is_simple_detail_page = false;
            bool is_all_detail_page = false;
            std::vector<SmoothUIToolKit::Transition2D> transition_list_buffer;
            int8_t simple_page_special_line_index = 0;
        };
        Data_t _data;
        Config_t _config;
        void _update_mode_label();
        void _render();

    public:
        PmDataPage();
        ~PmDataPage();
        inline Config_t& setConfig() { return _config; }
        void reset();
        void update(uint32_t currentTime);

        // Special pages
        void goSimpleDetailPage();
        void quitSimpleDetailPage();
        void goMoreDetailPage();
        void quitMoreDetailPage();

        inline bool want2quit() { return _data.want_to_quit; }
    };
} // namespace VIEWS
