/**
 * @file notice_page.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-01
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "hal/hal.h"
#include "misc.h"
#include "../../inputs/inputs.h"
#include "../../../../../assets/assets.h"
#include <cstdint>
#include <mooncake.h>
#include <smooth_ui_toolkit.h>

using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

bool SYSTEM::UI::CreateNoticePage(const std::string& title,
                                  std::function<void(Transition2D& transition)> contentRenderCallback,
                                  std::function<bool()> customInputCallback)
{
    spdlog::info("create notice page");
    bool is_button_encoder_clicked = false;

    Transition2D transition;
    transition.setDuration(500);
    transition.setTransitionPath(EasingPath::easeOutBack);
    transition.jumpTo(0, 180);
    transition.moveTo(0, 0);

    HAL::GetCanvas()->setTextScroll(false);
    uint32_t ssss = 0;
    while (!transition.isFinish())
    {
        transition.update(HAL::Millis());

        HAL::GetCanvas()->fillRect(transition.getValue().x,
                                   transition.getValue().y,
                                   240,
                                   240,
                                   AssetPool::GetStaticAsset()->Color.AppSettings.background);

        // Titile
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->setTextDatum(top_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppSettings.optionText);
        // AssetPool::LoadFont24(HAL::GetCanvas());
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);

        HAL::GetCanvas()->fillRect(
            0, transition.getValue().y, HAL::GetCanvas()->width(), 30, AssetPool::GetStaticAsset()->Color.AppSettings.selector);
        HAL::GetCanvas()->drawString(title.c_str(), HAL::GetCanvas()->width() / 2, transition.getValue().y);

        // Content
        HAL::GetCanvas()->setTextSize(1);
        HAL::GetCanvas()->setTextDatum(top_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppSettings.optionText);
        AssetPool::LoadFont16(HAL::GetCanvas());
        HAL::GetCanvas()->setCursor(0, transition.getValue().y + 40);

        contentRenderCallback(transition);

        HAL::CanvasUpdate();
    }

    // Wait click
    while (1)
    {
        if (customInputCallback != nullptr)
        {
            if (customInputCallback())
                break;
            continue;
        }

        HAL::FeedTheDog();
        HAL::Delay(50);

        Button::Update();
        if (Button::Side()->wasHold())
            break;

        if (Button::Encoder()->wasClicked())
        {
            is_button_encoder_clicked = true;
            break;
        }
    }

    spdlog::info("quit page");
    return is_button_encoder_clicked;
}
