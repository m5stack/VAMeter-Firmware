/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include "misc/water_wave_generator/water_wave_generator.h"
#include <functional>
#include <mooncake.h>
#include <smooth_ui_toolkit.h>
#include <string>
#include <vector>

namespace VIEW
{
    class LauncherView : public SmoothUIToolKit::SelectMenu::SmoothSelector
    {
    public:
        struct AppOptionProps_t
        {
            uint32_t themeColor = 0;
            void* icon = nullptr;
            std::string name;
        };

    private:
        struct Data_t
        {
            // Option props
            std::vector<AppOptionProps_t> launcher_option_props_list;

            // Selector color
            SmoothUIToolKit::SmoothRGB selector_color;
            bool is_selector_color_changed = true;

            // Input
            int encoder_last_count = 0;
            int last_selected_index = 0;

            // Auto open
            bool auto_open = true;
            bool is_auto_open_press_anim_on = false;
            uint32_t auto_open_time_count = 0;

            // Panel
            SmoothUIToolKit::Transition2D panel_transition;

            std::function<void(int selectedIndex)> app_open_callback = nullptr;

            int string_y_offset = 0;
        };
        Data_t _data;

    private:
        void _render_selector();
        void _render_options();
        void _render_panel();
        void _update_panel_anim();

        void _update_camera_keyframe() override;
        void onReadInput() override;
        void onRender() override;
        void onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime) override;
        void onGoLast() override;
        void onGoNext() override;
        void onClick() override;
        void onOpenEnd() override;

    public:
        void init();
        void addAppOption(const AppOptionProps_t& appOptionProps);
        inline void setAppOpenCallback(std::function<void(int selectedIndex)> appOpenCallback)
        {
            _data.app_open_callback = appOpenCallback;
        }
    };
} // namespace VIEW
