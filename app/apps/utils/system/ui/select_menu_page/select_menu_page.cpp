/**
 * @file select_menu_page.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "select_menu_page.h"
#include "../../../../../assets/assets.h"
#include "../../inputs/inputs.h"
#include <mooncake.h>

using namespace SYSTEM::UI;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

static constexpr int _selector_padding_x = 5;
static constexpr int _selector_padding_y = 5;
static constexpr int _selector_padding_2x = 10;
static constexpr int _selector_padding_2y = 10;
static constexpr int _selector_radius = 10;

static constexpr int _title_panel_height = 34;

static constexpr int _option_margin_top = 14;
static constexpr int _option_margin_left = 15;
static constexpr int _option_height = 24;

static constexpr int _option_open_anim_duration = 100;

void SelectMenuPage::init(const char* title, const std::vector<std::string>& optionList, int startIndex)
{
    // Selector
    setPositionDuration(200);
    setShapeDuration(400);
    setShapeTransitionPath(EasingPath::easeOutBack);

    // Slow down at the start up
    getSelectorPostion().jumpTo(-20, -20);
    getSelectorPostion().setDelay(100);
    getSelectorPostion().setUpdateCallback([](Transition2D* transition) {
        if (transition->isFinish())
        {
            transition->setDelay(0);
            transition->setUpdateCallback(nullptr);
        }
    });
    getSelectorShape().setDelay(100);
    getSelectorShape().setUpdateCallback([](Transition2D* transition) {
        if (transition->isFinish())
        {
            transition->setDelay(0);
            transition->setUpdateCallback(nullptr);
        }
    });

    // Camera
    setConfig().cameraSize.height = HAL::GetCanvas()->height();
    getCamera().setTransitionPath(EasingPath::easeOutBack);
    getCamera().setDuration(400);

    // Bg mask
    _data.transition_mask.setDelay(100);
    _data.transition_mask.setDuration(400);
    _data.transition_mask.jumpTo(-200, -120);
    _data.transition_mask.moveTo(120, 120);

    // Title panel
    _data.title_text = title;
    _data.transition_title_panel.setDelay(0);
    _data.transition_title_panel.setDuration(350);
    _data.transition_title_panel.jumpTo(0, -8 - 40);
    _data.transition_title_panel.moveTo(0, -8);

    // Create start up transitions
    _data.option_start_up_transition_list.resize(optionList.size());

    // Add option
    AssetPool::LoadFont24(HAL::GetCanvas());
    HAL::GetCanvas()->setTextColor(_theme.optionText);
    HAL::GetCanvas()->setTextDatum(top_left);
    for (int i = 0; i < optionList.size(); i++)
    {
        OptionProps_t new_option_props;

        new_option_props.userData = (void*)optionList[i].c_str();
        new_option_props.keyframe.x = _option_margin_left;
        new_option_props.keyframe.y = _title_panel_height + _option_margin_top + (_option_height + _option_margin_top) * i;
        new_option_props.keyframe.w = HAL::GetCanvas()->textWidth(optionList[i].c_str());
        new_option_props.keyframe.h = _option_height;

        addOption(new_option_props);

        // Set startup anim
        _data.option_start_up_transition_list[i].setDelay(i * 30 + 100);
        _data.option_start_up_transition_list[i].setDuration(400);
        _data.option_start_up_transition_list[i].setTransitionPath(EasingPath::easeOutBack);
        _data.option_start_up_transition_list[i].jumpTo(-240, new_option_props.keyframe.y - 24);
        _data.option_start_up_transition_list[i].moveTo(new_option_props.keyframe.x, new_option_props.keyframe.y);
    }

    // Reset encoder
    HAL::ResetEncoderCount();

    // Offset for efont font
    _data.string_y_offset = AssetPool::IsLocaleEn() ? 2 : 3;

    moveTo(startIndex);
}

/* -------------------------------------------------------------------------- */
/*                                    Input                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::onReadInput()
{
    if (isOpening())
        return;

    /* ------------------------- Scrolling with encoder ------------------------- */
    // Scroll encoder to switch option
    if (HAL::GetEncoderCount() > _data.encoder_last_count && !isPressing())
    {
        goNext();

        _data.encoder_last_count = HAL::GetEncoderCount();
    }
    else if (HAL::GetEncoderCount() < _data.encoder_last_count && !isPressing())
    {
        goLast();

        _data.encoder_last_count = HAL::GetEncoderCount();
    }

    /* ---------------------------------- Open ---------------------------------- */
    Button::Update();
    if (Button::Encoder()->wasPressed())
    {
        setConfig().moveInLoop = false;

        // Squeeze selector
        Vector4D_t squeeze_shape;
        squeeze_shape.w = getSelectedKeyframe().w * 5 / 6;
        squeeze_shape.h = getSelectedKeyframe().h * 2 / 3;
        squeeze_shape.x = getSelectedKeyframe().x + getSelectedKeyframe().w / 12;
        squeeze_shape.y = getSelectedKeyframe().y + getSelectedKeyframe().h / 6;
        press(squeeze_shape);
    }

    else if (Button::Encoder()->wasReleased())
    {
        release();
    }

    /* ---------------------------------- Back ---------------------------------- */
    else if (Button::Side()->wasHold())
    {
        _data.is_quiting = true;
    }
}

/* -------------------------------------------------------------------------- */
/*                                   Render                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::_render_selector()
{
    HAL::GetCanvas()->fillSmoothRoundRect(getSelectorCurrentFrame().x - _selector_padding_x,
                                          getSelectorCurrentFrame().y - _selector_padding_y + 3 - getCameraOffset().y,
                                          getSelectorCurrentFrame().w + _selector_padding_2x,
                                          getSelectorCurrentFrame().h + _selector_padding_2y,
                                          isOpening() ? _selector_radius * 4 : _selector_radius,
                                          _theme.selector);
}

void SelectMenuPage::_render_options()
{
    HAL::GetCanvas()->setTextDatum(top_left);
    if (_data.is_start_up_anim_done)
    {
        // Static
        for (int i = 0; i < getOptionList().size(); i++)
        {
            if (_data.custom_option_render_callback != nullptr)
            {
                _data.custom_option_render_callback(i,
                                                    getOptionList()[i].keyframe.x,
                                                    getOptionList()[i].keyframe.y - getCameraOffset().y +
                                                        _data.string_y_offset);
                continue;
            }

            HAL::GetCanvas()->drawString((const char*)getOptionList()[i].userData,
                                         getOptionList()[i].keyframe.x,
                                         getOptionList()[i].keyframe.y - getCameraOffset().y + _data.string_y_offset);
        }
    }
    else
    {
        // Anim
        for (int i = 0; i < getOptionList().size(); i++)
        {
            if (_data.custom_option_render_callback != nullptr)
            {
                _data.custom_option_render_callback(i,
                                                    _data.option_start_up_transition_list[i].getValue().x,
                                                    _data.option_start_up_transition_list[i].getValue().y -
                                                        getCameraOffset().y + _data.string_y_offset);
                continue;
            }

            HAL::GetCanvas()->drawString((const char*)getOptionList()[i].userData,
                                         _data.option_start_up_transition_list[i].getValue().x,
                                         _data.option_start_up_transition_list[i].getValue().y - getCameraOffset().y +
                                             _data.string_y_offset);
        }
    }
}

void SelectMenuPage::_render_title()
{
    HAL::GetCanvas()->fillSmoothRoundRect(-7, _data.transition_title_panel.getValue().y, 254, 40, 18, _theme.selector);

    HAL::GetCanvas()->setTextDatum(top_center);
    HAL::GetCanvas()->drawString(_data.title_text, 120, _data.transition_title_panel.getValue().y + 9 + _data.string_y_offset);
}

void SelectMenuPage::onRender()
{
    // Background
    if (_data.transition_mask.isFinish())
    {
        HAL::GetCanvas()->fillScreen(_theme.background);
    }
    else
    {
        HAL::GetCanvas()->fillScreen(_theme.selector);
        HAL::GetCanvas()->fillSmoothCircle(
            _data.transition_mask.getValue().x, _data.transition_mask.getValue().y, 200, _theme.background);
    }
    // HAL::GetCanvas()->fillScreen(_theme.background);

    if (isOpening())
    {
        _render_options();
        _render_title();
        _render_selector();
    }
    else
    {
        _render_selector();
        _render_options();
        _render_title();
    }

    // Callback
    onPostRender();

    // Push
    HAL::CanvasUpdate();
}

/* -------------------------------------------------------------------------- */
/*                                   Camera                                   */
/* -------------------------------------------------------------------------- */
void SelectMenuPage::_update_camera_keyframe()
{
    if (_config.cameraSize.height == 0)
        return;

    // Check if selector's target frame is inside of camera
    int new_y_offset = getCameraOffset().y;

    // Top
    if (getSelectedKeyframe().y - _selector_padding_y - _option_height < new_y_offset)
        new_y_offset = getSelectedKeyframe().y - _option_margin_top - _option_height - _option_margin_top;

    // Bottom
    else if (getSelectedKeyframe().y + _selector_padding_y + 3 + getSelectedKeyframe().h >
             new_y_offset + _config.cameraSize.height)
        new_y_offset = getSelectedKeyframe().y + getSelectedKeyframe().h - _config.cameraSize.height + _option_margin_top +
                       _selector_padding_y;

    getCamera().moveTo(0, new_y_offset);
}

void SelectMenuPage::onPress()
{
    // Make postion and shape anim same
    setDuration(400);
    setTransitionPath(EasingPath::easeOutBack);
}

void SelectMenuPage::onClick()
{
    // // Open selector
    // Vector4D_t open_shape;
    // open_shape.w = HAL::GetCanvas()->width();
    // open_shape.h = HAL::GetCanvas()->height();
    // open_shape.x = 0;
    // open_shape.y = 0 + getCameraOffset().y;

    // setDuration(80);
    // setTransitionPath(EasingPath::easeOutQuad);
    // open(open_shape);

    // // Hide title
    // _data.transition_title_panel.setDelay(0);
    // _data.transition_title_panel.setDuration(80);
    // _data.transition_title_panel.moveTo(0, -8 - 40);

    open(getSelectedKeyframe());
}

void SelectMenuPage::onOpenEnd() { _data.is_selected = true; }

void SelectMenuPage::onUpdate(const SmoothUIToolKit::TimeSize_t& currentTime)
{
    // Update tile panel
    _data.transition_title_panel.update(currentTime);

    if (_data.is_start_up_anim_done && _data.transition_mask.isFinish())
        return;

    // Update bg mask
    _data.transition_mask.update(currentTime);

    // Update start up anim
    for (int i = 0; i < _data.option_start_up_transition_list.size(); i++)
    {
        _data.option_start_up_transition_list[i].update(currentTime);

        // Check the last anim
        if (i == _data.option_start_up_transition_list.size() - 1)
        {
            if (_data.option_start_up_transition_list[i].isFinish())
            {
                _data.is_start_up_anim_done = true;
                _data.option_start_up_transition_list.clear();
            }
        }
    }
}

int SelectMenuPage::CreateAndWaitResult(const char* title,
                                        const std::vector<std::string>& optionList,
                                        int startIndex,
                                        Theme_t* theme,
                                        std::function<void(int optionIndex, int x, int y)> customOptionRenderCallback)
{
    if (optionList.size() == 0)
    {
        spdlog::info("empty option list");
        return 0;
    }

    // Create menu
    auto select_menu = new SelectMenuPage;

    // Set configs
    if (theme != nullptr)
        select_menu->setTheme(*theme);
    select_menu->setCustomOptionRenderCallback(customOptionRenderCallback);

    select_menu->init(title, optionList, startIndex);

    /* -------------------------- Wait option selected -------------------------- */
    while (1)
    {
        HAL::FeedTheDog();
        select_menu->update(HAL::Millis());

        // Check selected
        if (select_menu->isSelected())
            break;

        // Check kill signal
        if (select_menu->isQuiting())
            break;
    }

    /* ---------------------------------- Free ---------------------------------- */
    auto is_quiting = select_menu->isQuiting();
    auto selected_index = select_menu->getSelectedOptionIndex();
    delete select_menu;

    if (is_quiting)
        return -1;
    return selected_index;
}

bool SYSTEM::UI::CreateConfirmPage(const char* title, bool defaultYes, SelectMenuPage::Theme_t* theme)
{
    spdlog::info("create confirm page");
    std::vector<std::string> option_list;
    option_list.resize(2);
    option_list[0] = AssetPool::GetText().AppSettings_Option_Yes;
    option_list[1] = AssetPool::GetText().AppSettings_Option_No;
    auto selected = SelectMenuPage::CreateAndWaitResult(title, option_list, defaultYes ? 0 : 1, theme);
    return (selected == 0);
}
