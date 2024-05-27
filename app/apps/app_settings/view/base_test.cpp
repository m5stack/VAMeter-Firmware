/**
 * @file base_test.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-05-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "apps/utils/system/inputs/encoder/encoder.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/core.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

void AppSettings::_on_page_base_test()
{
    spdlog::info("on page base test");

    HAL::BaseGroveStartTest();

    CreateNoticePage(
        "Base Test",
        [](Transition2D& transition) {
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->drawString("- Relay State -", 120, transition.getValue().y + 55);

            HAL::GetCanvas()->drawString("- Grove State -", 120, transition.getValue().y + 150);
            HAL::GetCanvas()->drawString("IO9: High", 120, transition.getValue().y + 175);
            HAL::GetCanvas()->drawString("IO8: High", 120, transition.getValue().y + 195);

            HAL::GetCanvas()->drawString("<    Switch Relay State    >", 120, transition.getValue().y + 228);

            AssetPool::LoadFont72(HAL::GetCanvas());
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->drawString("OFF", 120, transition.getValue().y + 100);
        },
        []() {
            HAL::FeedTheDog();
            HAL::Delay(50);

            Button::Update();
            if (Button::Side()->wasHold())
                return true;

            Encoder::Update();
            if (Encoder::WasMoved())
            {
                if (Encoder::GetDirection() < 0)
                    HAL::SetBaseRelay(true);
                else
                    HAL::SetBaseRelay(false);
            }

            if (!HAL::BaseGroveGetIoALevel())
                HAL::SetBaseRelay(true);
            else if (!HAL::BaseGroveGetIoBLevel())
                HAL::SetBaseRelay(false);

            HAL::GetCanvas()->fillRect(0, 30, 240, 210, AssetPool::GetColor().AppSettings.background);

            // Render
            AssetPool::LoadFont16(HAL::GetCanvas());
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->drawString("- Relay State -", 120, 55);

            HAL::GetCanvas()->drawString("- Grove State -", 120, 150);
            std::string string_buffer;
            string_buffer = fmt::format("IO9: {}", HAL::BaseGroveGetIoALevel() ? "High" : "Low");
            HAL::GetCanvas()->drawString(string_buffer.c_str(), 120, 175);
            string_buffer = fmt::format("IO8: {}", HAL::BaseGroveGetIoBLevel() ? "High" : "Low");
            HAL::GetCanvas()->drawString(string_buffer.c_str(), 120, 195);

            HAL::GetCanvas()->drawString("<    Switch Relay State    >", 120, 228);

            AssetPool::LoadFont72(HAL::GetCanvas());
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->drawString(HAL::GetBaseRelayState() ? "ON" : "OFF", 120, 100);

            HAL::CanvasUpdate();

            return false;
        });

    HAL::BaseGroveStopTest();
}
