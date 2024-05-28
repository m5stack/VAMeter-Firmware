/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace SYSTEM
{
    namespace UI
    {
        class WaveProgressBarPage
        {
        public:
            struct Config_t
            {
                int min = 0;
                int max = 100;
                int step = 1;

                std::function<void(SmoothUIToolKit::Transition3D& transition)> onPostRender = nullptr;
                std::function<void(const int& targetValue)> onValueChanged = nullptr;
                std::function<void(const int& value)> onValueChangeTransition = nullptr;
            };

            struct Theme_t
            {
                uint32_t colorBackground = 0xFFFFFF;
                uint32_t colorWaveA = 0xC4BBFF;
                uint32_t colorWaveB = 0xDCD6FF;
                uint32_t colorText = 0x50478A;
            };

        private:
            struct Data_t
            {
                uint32_t wave_update_time_count = 0;
                uint32_t render_update_time_count = 0;
                uint32_t input_update_time_count = 0;

                int defaultValue = 50;
                int targetValue = 50;
                int encoder_last_count = 0;

                // X for value, Y for wave, Z for bg mask
                SmoothUIToolKit::Transition3D transition;
                SmoothUIToolKit::Misc::WaterWaveGenerator wave_generator;

                bool is_just_create = true;
                bool is_value_changed = false;
                bool is_quiting = false;
            };
            Data_t _data;
            Config_t _config;
            Theme_t _theme;
            void _update_wave(const uint32_t& currentTime);
            void _update_render(const uint32_t& currentTime);
            void _update_input(const uint32_t& currentTime);
            int _get_wave_y();

        public:
            inline Config_t& setConfig() { return _config; }
            inline Theme_t& setTheme() { return _theme; }
            inline void setTheme(const Theme_t& theme) { _theme = theme; }

            inline bool isQuiting() { return _data.is_quiting; }
            inline int targetValue() { return _data.targetValue; }

            void init(int defaultValue = 50);
            void update(const uint32_t& currentTime);

        public:
            /**
             * @brief Create a wave progress bar page and block until quit.
             *
             * @param labelText
             * @param onValueChanged
             * @param onValueChangeTransition
             * @param step
             * @param defaultValue
             * @param maxValue
             * @param minValue
             * @param theme
             * @return int
             */
            static int CreateAndWaitResult(const std::string& labelText,
                                           std::function<void(const int& targetValue)> onValueChanged = nullptr,
                                           std::function<void(const int& value)> onValueChangeTransition = nullptr,
                                           int step = 1,
                                           int defaultValue = 50,
                                           int maxValue = 100,
                                           int minValue = 0,
                                           Theme_t* theme = nullptr);
        };

        /**
         * @brief Create a brightness setting page.
         *
         * @return true changed
         * @return false
         */
        bool CreatBrightnessSettingPage();
    } // namespace UI
} // namespace SYSTEM
