/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "wave_progress_bar_page.h"
#include "../../inputs/inputs.h"
#include "../../../../../hal/hal.h"
#include "../../../../../assets/assets.h"
#include <smooth_ui_toolkit.h>
#include <mooncake.h>
#include <cstdint>
#include <string>

using namespace SYSTEM::UI;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

static constexpr uint32_t _input_update_interval = 20;
static constexpr uint32_t _wave_update_interval = 10;
static constexpr uint32_t _render_update_interval = 15;

// Map value into canvas height
int WaveProgressBarPage::_get_wave_y() { return 240 - 240 * (_data.targetValue - _config.min) / (_config.max - _config.min); }

void WaveProgressBarPage::init(int defaultValue)
{
    _data.defaultValue = defaultValue;
    _data.targetValue = defaultValue;

    if (_config.min > _config.max)
        std::swap(_config.min, _config.max);

    // Setup transition
    _data.transition.jumpTo(0, 240, 320);
    _data.transition.moveTo(_data.targetValue, _get_wave_y(), 120);

    // Value transition
    _data.transition.getXTransition().setDuration(300);

    // Wave transition
    _data.transition.getYTransition().setDuration(600);
    _data.transition.getYTransition().setTransitionPath(EasingPath::easeOutBack);

    // Background transition
    _data.transition.getZTransition().setDuration(600);
    _data.transition.getZTransition().setTransitionPath(EasingPath::easeOutBack);

    // Wave gen
    _data.wave_generator.init(HAL::GetCanvas()->width());

    // Reset
    HAL::ResetEncoderCount();
}

void WaveProgressBarPage::update(const uint32_t& currentTime)
{
    _data.transition.update(currentTime);
    _update_input(currentTime);
    _update_wave(currentTime);
    _update_render(currentTime);
}

void WaveProgressBarPage::_update_wave(const uint32_t& currentTime)
{
    if (currentTime - _data.wave_update_time_count > _wave_update_interval)
    {
        // Make it faster
        _data.wave_generator.update();
        _data.wave_generator.update();
        _data.wave_update_time_count = currentTime;
    }
}

void WaveProgressBarPage::_update_render(const uint32_t& currentTime)
{
    if (currentTime - _data.render_update_time_count > _render_update_interval)
    {
        /* ----------------------------------- Bg ----------------------------------- */
        if (_data.transition.getZTransition().isFinish())
            HAL::GetCanvas()->fillScreen(_theme.colorBackground);
        else
            HAL::GetCanvas()->fillSmoothCircle(120, _data.transition.getZTransition().getValue(), 200, _theme.colorBackground);

        /* ---------------------------------- Wave ---------------------------------- */
        int wave_x = 0;
        int wave_y_offset = 0;
        _data.wave_generator.getWaveB().peekAll([&](const int& wave_y) {
            wave_y_offset = wave_y + _data.transition.getYTransition().getValue();
            HAL::GetCanvas()->drawFastVLine(
                wave_x, wave_y_offset, HAL::GetCanvas()->height() - wave_y_offset, _theme.colorWaveB);
            wave_x++;
        });

        wave_x = 0;
        _data.wave_generator.getWaveA().peekAll([&](const int& wave_y) {
            wave_y_offset = wave_y + _data.transition.getYTransition().getValue();
            HAL::GetCanvas()->drawFastVLine(
                wave_x, wave_y_offset, HAL::GetCanvas()->height() - wave_y_offset, _theme.colorWaveA);
            wave_x++;
        });

        /* ---------------------------------- Push ---------------------------------- */
        if (_config.onPostRender != nullptr)
            _config.onPostRender(_data.transition);
        HAL::CanvasUpdate();

        // Reset
        _data.render_update_time_count = currentTime;
    }
}

void WaveProgressBarPage::_update_input(const uint32_t& currentTime)
{
    // Hold until startup anim finish
    if (_data.is_just_create)
    {
        if (_data.transition.isFinish())
            _data.is_just_create = false;
        return;
    }

    // Callback
    if (_config.onValueChangeTransition != nullptr && !_data.transition.isFinish())
        _config.onValueChangeTransition(_data.transition.getXTransition().getValue());

    if (currentTime - _data.input_update_time_count > _input_update_interval)
    {
        Button::Update();

        // Reset
        if (Button::Side()->wasClicked())
        {
            _data.targetValue = _data.defaultValue;
            _data.is_value_changed = true;
        }

        // Quit
        else if (Button::Side()->wasHold() || Button::Encoder()->wasClicked())
        {
            _data.is_quiting = true;
        }

        // Scroll encoder to change target value
        if (HAL::GetEncoderCount() > _data.encoder_last_count)
        {
            _data.targetValue += _config.step;
            _data.encoder_last_count = HAL::GetEncoderCount();
            _data.is_value_changed = true;
        }
        else if (HAL::GetEncoderCount() < _data.encoder_last_count)
        {
            _data.targetValue -= _config.step;
            _data.encoder_last_count = HAL::GetEncoderCount();
            _data.is_value_changed = true;
        }

        if (_data.is_value_changed)
        {
            _data.is_value_changed = false;

            // Clamp value
            if (_data.targetValue > _config.max)
                _data.targetValue = _config.max;
            else if (_data.targetValue < _config.min)
                _data.targetValue = _config.min;

            // Callback
            if (_config.onValueChanged != nullptr)
                _config.onValueChanged(_data.targetValue);

            // Update transition
            _data.transition.moveTo(_data.targetValue, _get_wave_y(), _data.transition.getZTransition().getEndValue());
        }

        // Reset
        _data.input_update_time_count = currentTime;
    }
}

int WaveProgressBarPage::CreateAndWaitResult(const std::string& labelText,
                                             std::function<void(const int& targetValue)> onValueChanged,
                                             std::function<void(const int& value)> onValueChangeTransition,
                                             int step,
                                             int defaultValue,
                                             int maxValue,
                                             int minValue,
                                             Theme_t* theme)
{
    auto wave_progress_bar = new WaveProgressBarPage;

    wave_progress_bar->setConfig().onValueChanged = onValueChanged;
    wave_progress_bar->setConfig().onValueChangeTransition = onValueChangeTransition;
    wave_progress_bar->setConfig().step = step;
    wave_progress_bar->setConfig().max = maxValue;
    wave_progress_bar->setConfig().min = minValue;

    // Render labels
    HAL::GetCanvas()->setTextSize(1);
    HAL::GetCanvas()->setTextDatum(top_center);
    HAL::GetCanvas()->setTextColor(theme == nullptr ? (uint32_t)0x50478A : theme->colorText);
    wave_progress_bar->setConfig().onPostRender = [&](Transition3D& transion) {
        // Label
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_24);
        HAL::GetCanvas()->drawString(labelText.c_str(), 120, transion.getZTransition().getValue() - 62);

        // Value
        HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
        HAL::GetCanvas()->drawString(
            std::to_string(transion.getXTransition().getValue()).c_str(), 120, transion.getZTransition().getValue() - 42);
    };

    wave_progress_bar->init(defaultValue);

    if (theme != nullptr)
        wave_progress_bar->setTheme(*theme);

    while (1)
    {
        HAL::FeedTheDog();
        wave_progress_bar->update(HAL::Millis());

        // Check kill signal
        if (wave_progress_bar->isQuiting())
            break;
    }

    int result = wave_progress_bar->targetValue();
    delete wave_progress_bar;

    return result;
}

bool SYSTEM::UI::CreatBrightnessSettingPage()
{
    // spdlog::info("create brightness setting page");

    // 50 ~ 255
    int step = (255 - 50) / 5;

    auto startup_value = (HAL::GetDisplay()->getBrightness() - 50) / step;
    auto last_value = startup_value;

    WaveProgressBarPage::Theme_t theme;
    theme.colorBackground = AssetPool::GetColor().AppSettings.background;
    theme.colorText = AssetPool::GetColor().AppSettings.optionText;

    WaveProgressBarPage::CreateAndWaitResult(
        "BRIGHTNESS",
        nullptr,
        [&last_value, &step](const int& value) {
            if (value != last_value)
            {
                spdlog::info("{}", 50 + value * step);
                HAL::GetDisplay()->setBrightness(50 + value * step);
                last_value = value;
            }
        },
        1,
        startup_value,
        5,
        1,
        &theme);

    return (startup_value != (HAL::GetDisplay()->getBrightness() - 50) / step);
}
