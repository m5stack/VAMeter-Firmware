/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "notification_bubble.h"
#include "../../../../../assets/assets.h"
#include <mooncake.h>

using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;

static constexpr int _panel_startup_x = 84;
static constexpr int _panel_startup_y = -36;
static constexpr int _panel_startup_w = 72;
static constexpr int _panel_startup_h = 24;
static constexpr int _panel_x = 10;
static constexpr int _panel_y = 16;
static constexpr int _panel_w = 220;
static constexpr int _panel_h = 48;
static constexpr int _panel_r = 15;

void NotificationBubble::onReset() { _data.duration_count = HAL::Millis(); }

void NotificationBubble::onPopOut()
{
    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);

    getTransition().setEachDelay(70, 0, 70, 70);
    getTransition().setEachDuration(600, 500, 600, 600);
    getTransition().setTransitionPath(EasingPath::easeOutBack);
}

void NotificationBubble::onHide()
{
    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);

    getTransition().setEachDelay(0, 70, 0, 0);
    getTransition().setEachDuration(400, 300, 400, 400);
    getTransition().setTransitionPath(EasingPath::easeOutBack);
}

void NotificationBubble::onUpdate()
{
    if (isHidden() || isHidding())
        return;

    if (HAL::Millis() - _data.duration_count > _data.duration)
    {
        spdlog::info("bubble timeout");
        hide();
    }
}

void NotificationBubble::onRender()
{
    AssetPool::LoadFont24(HAL::GetCanvas());

    // Panel
    auto frame = getTransition().getValue();
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, TFT_WHITE);

    // Msg
    if (isHidding())
        return;
    HAL::GetCanvas()->setTextColor((uint32_t)0x3C3C3C);
    HAL::GetCanvas()->setTextDatum(middle_center);
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->drawString(_data.msg.c_str(), frame.x + frame.w / 2, frame.y + frame.h / 2 + 1);
}

void NotificationBubble::config(std::string msg, uint32_t duration)
{
    _data.msg = msg;
    _data.duration = duration;
}

static NotificationBubble* _bubble = nullptr;

void NotificationBubble::Push(std::string msg, uint32_t duration)
{
    spdlog::info("push notify: {}, {}", msg, duration);

    if (_bubble != nullptr)
        delete _bubble;
    _bubble = new NotificationBubble;

    _bubble->config(msg, duration);
    _bubble->reset();
    _bubble->popOut();
}

void NotificationBubble::UpdateAndRender()
{
    if (_bubble == nullptr)
        return;

    _bubble->update(HAL::Millis());

    // Auto free
    if (_bubble->isHidden())
    {
        spdlog::info("free bubble");
        delete _bubble;
        _bubble = nullptr;
    }
}

void NotificationBubble::Free()
{
    if (_bubble == nullptr)
        return;
    delete _bubble;
    _bubble = nullptr;
}
