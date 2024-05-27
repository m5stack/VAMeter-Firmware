/**
 * @file misc.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <smooth_ui_toolkit.h>

namespace SYSTEM
{
    namespace UI
    {
        /**
         * @brief A hook instance to render simple logging page
         *
         * @param log
         * @param pushScreen
         * @param clear
         */
        void OnLogPageRender(const std::string& log, bool pushScreen, bool clear);

        /**
         * @brief A hook instance to render ota logging page
         *
         * @param log
         * @param pushScreen
         * @param clear
         */
        void OnOtaLogPageRender(const std::string& log, bool pushScreen, bool clear);

        /**
         * @brief Create a notice page
         *
         * @param title
         * @param contentRenderCallback
         * @param customInputCallback
         * @return true
         * @return false
         */
        bool CreateNoticePage(const std::string& title,
                              std::function<void(SmoothUIToolKit::Transition2D& transition)> contentRenderCallback,
                              std::function<bool()> customInputCallback = nullptr);
    } // namespace UI
} // namespace SYSTEM
