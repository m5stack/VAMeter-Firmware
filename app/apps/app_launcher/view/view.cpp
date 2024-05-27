/**
 * @file view.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-01-18
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "view.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include "../app_launcher.h"
#include <cstdint>
#include <mooncake.h>

using namespace SmoothUIToolKit;
using namespace SmoothUIToolKit::SelectMenu;
using namespace MOONCAKE::APPS;
using namespace VIEW;
using namespace SYSTEM::INPUTS;

// #define _PASS_ICON_IMAGE

/* -------------------------------------------------------------------------- */
/*                                   Config                                   */
/* -------------------------------------------------------------------------- */
static constexpr int _icon_height = 128;
static constexpr int _icon_width = 128;
static constexpr int _icon_gap_x = 28;
static constexpr int _icon_gap_y = 36;
static constexpr int _icon_radius = 32;
static constexpr int _icon_image_margin_top = 14;
static constexpr int _icon_image_margin_left = 14;
static constexpr int _icon_image_width = 100;
static constexpr int _icon_image_height = 100;

static constexpr int _selector_padding_x = 12;
static constexpr int _selector_padding_y = 12;
static constexpr int _selector_padding_2x = 24;
static constexpr int _selector_padding_2y = 24;
static constexpr int _selector_radius = 42;

static constexpr int _auto_open_time = 1500;
static constexpr int _auto_open_press_anim_time = 1100;

// For restoration to last opened
static int _last_selected_option_index = -1;

/* -------------------------------------------------------------------------- */
/*                                    Setup                                   */
/* -------------------------------------------------------------------------- */
void LauncherView::init()
{
    // Reset
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->setTextDatum(top_center);
    // HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppLauncher.iconBackgound);
    HAL::ResetEncoderCount();
    _data.auto_open_time_count = HAL::Millis();

    // Selector color
    _data.selector_color.jumpTo(Hex2Rgb(AssetPool::GetColor().AppLauncher.background));
    _data.selector_color.moveTo(Hex2Rgb(AssetPool::GetColor().AppLauncher.selector));
    _data.selector_color.setDuration(350);

    // Selector transition
    setDuration(500);
    setTransitionPath(EasingPath::easeOutBack);

    // Panel
    _data.panel_transition.jumpTo(0, HAL::GetCanvas()->height());
    _data.panel_transition.moveTo(0, 203);
    _data.panel_transition.setDelay(300);
    _data.panel_transition.setTransitionPath(EasingPath::easeOutBack);
    _data.panel_transition.setDuration(400);

    // Camera
    getCamera().setDuration(600);
    getCamera().setTransitionPath(EasingPath::easeOutBack);
    getCamera().jumpTo(-120, 0);

    // If need restoration
    if (_last_selected_option_index != -1)
    {
        // Disable auto opening
        _data.auto_open = false;

        // Jump to selected option
        jumpTo(_last_selected_option_index);
        _data.last_selected_index = _last_selected_option_index;

        // Jump to selector color
        _data.selector_color.jumpTo(Hex2Rgb(_data.launcher_option_props_list[getSelectedOptionIndex()].themeColor));
        _data.selector_color.moveTo(Hex2Rgb(AssetPool::GetColor().AppLauncher.selector));

        // Jump to selected camera
        SmoothSelector::_data.camera_offset.jumpTo(getSelectedKeyframe().x - _icon_gap_x * 2, 0);

        // Jump to opened anim
        getSelectorPostion().jumpTo(getSelectedKeyframe().x - _icon_gap_x * 2, 0);
        getSelectorShape().jumpTo(HAL::GetCanvas()->width(), HAL::GetCanvas()->height());
    }

    // Offset for efont font
    _data.string_y_offset = AssetPool::IsLocaleEn() ? 2 : 3;
}

void LauncherView::addAppOption(const AppOptionProps_t& appOptionProps)
{
    _data.launcher_option_props_list.push_back(appOptionProps);

    // Add menu option
    OptionProps_t new_props;
    new_props.keyframe.x = (_data.launcher_option_props_list.size() - 1) * (_icon_width + _icon_gap_x) + _icon_gap_x * 2;
    new_props.keyframe.y = _icon_gap_y;
    new_props.keyframe.w = _icon_width;
    new_props.keyframe.h = _icon_height;
    addOption(new_props);
}

/* -------------------------------------------------------------------------- */
/*                                Handle inputs                               */
/* -------------------------------------------------------------------------- */
void LauncherView::onReadInput()
{
    if (isOpening())
        return;

    /* ------------------------- Scrolling with encoder ------------------------- */
    // Scroll encoder to switch option, also disable auto open
    if (HAL::GetEncoderCount() > _data.encoder_last_count && !isPressing())
    {
        setConfig().moveInLoop = false;
        goNext();

        _data.auto_open = false;
        _data.encoder_last_count = HAL::GetEncoderCount();
    }
    else if (HAL::GetEncoderCount() < _data.encoder_last_count && !isPressing())
    {
        setConfig().moveInLoop = false;
        goLast();

        _data.auto_open = false;
        _data.encoder_last_count = HAL::GetEncoderCount();
    }

    /* --------------------- Open and scrolling with buttons -------------------- */
    Button::Update();

    // Press encoder to open option
    if (Button::Encoder()->wasPressed())
    {
        // Squeeze selector
        Vector4D_t squeeze_shape;
        squeeze_shape.w = getSelectedKeyframe().w * 3 / 4;
        squeeze_shape.h = getSelectedKeyframe().h * 3 / 4;
        squeeze_shape.x = getSelectedKeyframe().x + getSelectedKeyframe().w / 8;
        squeeze_shape.y = getSelectedKeyframe().y + getSelectedKeyframe().h / 8;
        press(squeeze_shape);

        _data.auto_open = false;
        setConfig().moveInLoop = false;
    }

    else if (Button::Encoder()->wasReleased())
    {
        release();
    }

    else if (Button::Side()->wasClicked() && Button::Encoder()->isReleased())
    {
        _data.auto_open = true;
        setConfig().moveInLoop = true;

        goNext();
    }

    /* -------------------------- Handle auto openning -------------------------- */
    if (_data.auto_open)
    {
        // Squeeze selector to notice it's about to auto open
        if (!_data.is_auto_open_press_anim_on && (HAL::Millis() - _data.auto_open_time_count > _auto_open_press_anim_time))
        {
            _data.is_auto_open_press_anim_on = true;

            // Squeeze selector
            Vector4D_t squeeze_shape;
            squeeze_shape.w = getSelectedKeyframe().w * 3 / 4;
            squeeze_shape.h = getSelectedKeyframe().h * 3 / 4;
            squeeze_shape.x = getSelectedKeyframe().x + getSelectedKeyframe().w / 8;
            squeeze_shape.y = getSelectedKeyframe().y + getSelectedKeyframe().h / 8;
            getSelectorPostion().moveTo(squeeze_shape.x, squeeze_shape.y);
            getSelectorShape().reshapeTo(squeeze_shape.w, squeeze_shape.h);
        }

        if (HAL::Millis() - _data.auto_open_time_count > _auto_open_time)
        {
            onClick();
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void LauncherView::_render_selector()
{
    HAL::GetCanvas()->fillSmoothRoundRect(getSelectorCurrentFrame().x - _selector_padding_x - getCameraOffset().x,
                                          getSelectorCurrentFrame().y - _selector_padding_y,
                                          getSelectorCurrentFrame().w + _selector_padding_2x,
                                          getSelectorCurrentFrame().h + _selector_padding_2y,
                                          _selector_radius,
                                          Rgb2Hex(_data.selector_color.getValue()));
}

void LauncherView::_render_options()
{
    for (int i = 0; i < getOptionList().size(); i++)
    {
        // Icon background
        HAL::GetCanvas()->fillSmoothRoundRect(getOptionList()[i].keyframe.x - getCameraOffset().x,
                                              getOptionList()[i].keyframe.y,
                                              getOptionList()[i].keyframe.w,
                                              getOptionList()[i].keyframe.h,
                                              _icon_radius,
                                              AssetPool::GetColor().AppLauncher.iconBackgound);

#ifndef _PASS_ICON_IMAGE
        // Icon image
        if ((_data.launcher_option_props_list)[i].icon == nullptr)
            continue;
        HAL::GetCanvas()->pushImage(getOptionList()[i].keyframe.x + _icon_image_margin_left - getCameraOffset().x,
                                    getOptionList()[i].keyframe.y + _icon_image_margin_top,
                                    _icon_image_width,
                                    _icon_image_height,
                                    (uint16_t*)(_data.launcher_option_props_list)[i].icon);
#endif
    }
}

void LauncherView::_render_panel()
{
    HAL::GetCanvas()->fillSmoothRoundRect(
        -7, _data.panel_transition.getValue().y, 254, 45, 18, AssetPool::GetColor().AppLauncher.panel);

    if (_data.panel_transition.getYTransition().getEndValue() != HAL::GetCanvas()->height())
        HAL::GetCanvas()->drawString(_data.launcher_option_props_list[getSelectedOptionIndex()].name.c_str(),
                                     120,
                                     _data.panel_transition.getValue().y + 4 + _data.string_y_offset);

    HAL::GetCanvas()->pushImage(
        215, _data.panel_transition.getValue().y + 10, 20, 20, AssetPool::GetImage().AppLauncher.next_icon);
}

void LauncherView::onRender()
{
    // Clear
    HAL::GetCanvas()->fillScreen(AssetPool::GetColor().AppLauncher.background);

    if (isOpening())
    {
        _render_options();
        _render_panel();
        _render_selector();
    }
    else
    {
        _render_selector();
        _render_options();
        _render_panel();
    }

    // Camera
    // spdlog::info("{}", getCameraOffset().x);
    // HAL::GetCanvas()->drawFastVLine(getCameraOffset().x, 0, 240, TFT_GREEN);

    // HAL::RenderFpsPanel();

    // Push
    HAL::CanvasUpdate();
}

/* -------------------------------------------------------------------------- */
/*                                   Update                                   */
/* -------------------------------------------------------------------------- */
void LauncherView::onUpdate(const TimeSize_t& currentTime)
{
    _data.selector_color.update(currentTime);
    _data.panel_transition.update(currentTime);
}

/* -------------------------------------------------------------------------- */
/*                                 Navigation                                 */
/* -------------------------------------------------------------------------- */
void LauncherView::_update_panel_anim()
{
    if (getSelectedOptionIndex() == _data.last_selected_index)
        return;
    _data.last_selected_index = getSelectedOptionIndex();

    // Hide panel
    _data.panel_transition.setDelay(0);
    _data.panel_transition.setDuration(200);
    _data.panel_transition.moveTo(0, HAL::GetCanvas()->height());
    _data.panel_transition.setUpdateCallback([](Transition2D* transition) {
        // Pop up again when finished
        if (transition->isFinish())
        {
            transition->setDelay(200);
            transition->moveTo(0, 203);
            transition->setDuration(400);
        }
    });
}

void LauncherView::onGoLast()
{
    _data.auto_open_time_count = HAL::Millis();
    _data.is_auto_open_press_anim_on = false;

    _update_panel_anim();
}
void LauncherView::onGoNext()
{
    _data.auto_open_time_count = HAL::Millis();
    _data.is_auto_open_press_anim_on = false;

    _update_panel_anim();
}

/* -------------------------------------------------------------------------- */
/*                                  App Open                                  */
/* -------------------------------------------------------------------------- */
void LauncherView::onClick()
{
    // Play open anim
    open(Vector4D_t(0 + getCameraOffset().x, 0, HAL::GetCanvas()->width(), HAL::GetCanvas()->height()));
    _data.selector_color.jumpTo(Hex2Rgb(AssetPool::GetColor().AppLauncher.selector));
    _data.selector_color.moveTo(Hex2Rgb(_data.launcher_option_props_list[getSelectedOptionIndex()].themeColor));

    // Open quicker
    setDuration(350);
}

void LauncherView::onOpenEnd()
{
    // Callback
    if (_data.app_open_callback != nullptr)
        _data.app_open_callback(getSelectedOptionIndex());

    // Store selected option for restoration in next create
    _last_selected_option_index = getSelectedOptionIndex();
    // close();
    // Launcher view (this) will be destroy on next mooncake update, no need to close anymore
}

/* -------------------------------------------------------------------------- */
/*                                   Camera                                   */
/* -------------------------------------------------------------------------- */
void LauncherView::_update_camera_keyframe()
{
    // Get a offset to align icon to center
    if (getSelectedKeyframe().x != getCameraOffset().x)
    {
        // spdlog::info("go {}", getSelectedKeyframe().x);
        SmoothSelector::_data.camera_offset.moveTo(getSelectedKeyframe().x - _icon_gap_x * 2, 0);
    }
}
