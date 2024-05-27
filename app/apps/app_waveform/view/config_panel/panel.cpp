/**
 * @file config_panel.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-05-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "config_panel.h"
#include <cstring>
#include <mooncake.h>
#include "../../../../hal/hal.h"
#include "../../triggers/triggers.h"
#include "../../../utils/system/system.h"
#include "../../../../assets/assets.h"

using namespace SmoothUIToolKit::Widgets;

static constexpr int _panel_startup_x = 360;
static constexpr int _panel_startup_y = 185;
static constexpr int _panel_startup_w = 5;
static constexpr int _panel_startup_h = 20;
static constexpr int _panel_x = 0;
static constexpr int _panel_y = 0;
static constexpr int _panel_w = 240;
static constexpr int _panel_h = 240;
static constexpr int _panel_r = 16;

static ConfigPanel::Config_t* _config_storage = nullptr;

ConfigPanel::~ConfigPanel()
{
    if (_data.selector != nullptr)
        delete _data.selector;

    // Store config
    if (_config_storage == nullptr)
        _config_storage = new Config_t;
    std::memcpy(_config_storage, &_data.config, sizeof(Config_t));
}

void ConfigPanel::onInit()
{
    // Restore storage
    if (_config_storage != nullptr)
        std::memcpy(&_data.config, _config_storage, sizeof(Config_t));

    setRenderOnUpdate(false);

    // Add configs
    addChild(new ConfigReacordTime(&_data.config));
    addChild(new ConfigTrigerMode(&_data.config));
    addChild(new ConfigThreshold(&_data.config));

    _data.selector = new ConfigSelector;
    _data.selector->init();
    _data.selector->enter(this);
    _data.selector->setRenderOnUpdate(false);
}

void ConfigPanel::popOut()
{
    SmoothWidgetBase::popOut();
    _data.selector->popOut();
}

void ConfigPanel::hide()
{
    SmoothWidgetBase::hide();
    _data.selector->hide();
}

void ConfigPanel::onPopOut()
{
    getTransition().jumpTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().moveTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().setDuration(300);
    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);

    getTransition().setDelay(0);
}

void ConfigPanel::onHide()
{
    constexpr int delay = 200;

    getTransition().jumpTo(_panel_x, _panel_y, _panel_w, _panel_h);
    getTransition().moveTo(_panel_startup_x, _panel_startup_y, _panel_startup_w, _panel_startup_h);
    getTransition().setDuration(300);
    // getTransition().setTransitionPath(SmoothUIToolKit::EasingPath::easeOutBack);
    getTransition().setDelay(delay);
}

void ConfigPanel::onRender()
{
    if (isHidden())
        return;

    // HAL::GetCanvas()->fillScreen(TFT_LIGHTGRAY);

    auto frame = getTransition().getValue();
    HAL::GetCanvas()->fillSmoothRoundRect(frame.x, frame.y, frame.w, frame.h, _panel_r, TFT_WHITE);

    _data.selector->render();
}

void ConfigPanel::onPostRender()
{
    // if (isHidden())
    //     return;
    // HAL::CanvasUpdate();
}

void ConfigPanel::onUpdate() { _data.selector->update(HAL::Millis()); }
