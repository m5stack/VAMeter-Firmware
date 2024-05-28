/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "config_panel.h"
#include <cstdint>
#include <mooncake.h>
#include "../../../../hal/hal.h"
#include "../../triggers/triggers.h"
#include "../../../utils/system/system.h"
#include "../../../../assets/assets.h"
#include "core/easing_path/easing_path.h"

using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
// static constexpr int _panel_x = 9;
// static constexpr int _panel_y = 175;
// static constexpr int _panel_w = 222;
// static constexpr int _panel_h = 44;
static constexpr int _panel_r = 15;
static constexpr int _stroke_width = 4;
static uint32_t _color_container = 0xCADED5;
static uint32_t _color_primary = 0x035E36;
static uint32_t _color_on_primary = 0xFFFFFF;

void ConfigSelector::onInit() { moveInloop(false); }

void ConfigSelector::onPopOut()
{
    constexpr int delay = 0;

    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 0, delay + 50, delay + 50, delay + 0);
    getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutQuad);

    getTransition().setUpdateCallback(
        [](Transition4D* transition)
        {
            if (transition->isFinish())
            {
                transition->setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
                transition->setDelay(0);
                transition->setDuration(400);
                transition->setUpdateCallback(nullptr);
            }
        });

    moveTo(0);
}

void ConfigSelector::onHide()
{
    int delay = 60 * (getOptionNum() - 1 - getHoveringOptionIndex());

    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    getTransition().setDuration(400);
    getTransition().setEachDelay(delay + 50, delay + 0, delay + 0, delay + 0);
    getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutQuad);

    if (isHoveringOptionSelected())
        unSelectOption();
}

void ConfigSelector::onRender()
{
    if (isHidden())
        return;

    // spdlog::info("rrr");

    auto frame = getTransition().getValue();
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x - _stroke_width,
                                          frame.y - _stroke_width,
                                          frame.w + _stroke_width * 2,
                                          frame.h + _stroke_width * 2,
                                          _panel_r,
                                          _color_primary);
}

void ConfigSelector::onUpdate()
{
    if (isHidden())
        return;

    // Navigate
    if (!isHoveringOptionSelected() && Encoder::WasMoved())
    {
        if (Encoder::GetDirection() > 0)
            goLast();
        else
            goNext();
    }

    // Select
    if (Button::Encoder()->wasClicked())
    {
        if (isHoveringOptionSelected())
            unSelectOption();
        else
            selectOption();
    }
}
