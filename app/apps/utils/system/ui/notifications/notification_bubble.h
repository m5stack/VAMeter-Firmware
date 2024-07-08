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
        class NotificationBubble : public SmoothUIToolKit::Widgets::SmoothWidgetBase
        {
        private:
            struct Data_t
            {
                std::string msg;
                uint32_t duration = 0;
                uint32_t duration_count = 0;
            };
            Data_t _data;

        public:
            void onReset() override;
            void onPopOut() override;
            void onHide() override;
            void onUpdate() override;
            void onRender() override;
            void config(std::string msg, uint32_t duration);

        public:
            static void Push(std::string msg, uint32_t duration = 1200);
            static void UpdateAndRender();
            static void Free();
        };
    } // namespace UI
} // namespace SYSTEM
