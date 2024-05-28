/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <smooth_ui_toolkit.h>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include "../../../../../assets/assets.h"

namespace SYSTEM
{
    namespace UI
    {
        class SelectMenuPage : public SmoothUIToolKit::SelectMenu::SmoothSelector
        {
        public:
            struct Theme_t
            {
                uint32_t background = AssetPool::GetColor().AppFile.background;
                uint32_t optionText = AssetPool::GetColor().AppFile.optionText;
                uint32_t selector = AssetPool::GetColor().AppFile.selector;
            };

        private:
            struct Data_t
            {
                int encoder_last_count = 0;
                int string_y_offset = 0;

                bool is_selected = false;
                bool is_quiting = false;

                bool is_start_up_anim_done = false;
                std::vector<SmoothUIToolKit::Transition2D> option_start_up_transition_list;

                const char* title_text = nullptr;

                SmoothUIToolKit::Transition2D transition_mask;
                SmoothUIToolKit::Transition2D transition_title_panel;

                std::function<void(int optionIndex, int x, int y)> custom_option_render_callback = nullptr;
            };
            Data_t _data;
            Theme_t _theme;
            void _update_camera_keyframe() override;
            void _render_selector();
            void _render_options();
            void _render_title();

        public:
            inline Theme_t& setTheme() { return _theme; }
            inline void setTheme(Theme_t theme) { _theme = theme; }
            inline void setCustomOptionRenderCallback(std::function<void(int optionIndex, int x, int y)> callback)
            {
                _data.custom_option_render_callback = callback;
            }

            void init(const char* title, const std::vector<std::string>& optionList, int startIndex = 0);
            void onReadInput() override;
            void onRender() override;
            void onPress() override;
            void onClick() override;
            void onOpenEnd() override;
            void onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime) override;
            inline bool& isSelected() { return _data.is_selected; }
            inline bool isQuiting() { return _data.is_quiting; }

        public:
            virtual void onPostRender() {}

        public:
            static int
            CreateAndWaitResult(const char* title,
                                const std::vector<std::string>& optionList,
                                int startIndex = 0,
                                Theme_t* theme = nullptr,
                                std::function<void(int optionIndex, int x, int y)> customOptionRenderCallback = nullptr);
        };

        bool CreateConfirmPage(const char* title, bool defaultYes = true, SelectMenuPage::Theme_t* theme = nullptr);
    } // namespace UI
} // namespace SYSTEM
