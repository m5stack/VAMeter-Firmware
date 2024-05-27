/**
 * @file hal.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hal.h"
#include <mooncake.h>
#include "../assets/assets.h"
#include <sstream>

HAL* HAL::_hal = nullptr;

HAL* HAL::Get() { return _hal; }

bool HAL::Check() { return _hal != nullptr; }

bool HAL::Inject(HAL* hal)
{
    if (_hal != nullptr)
    {
        spdlog::error("HAL already exist");
        return false;
    }

    if (hal == nullptr)
    {
        spdlog::error("invalid HAL ptr");
        return false;
    }

    hal->init();
    spdlog::info("HAL injected, type: {}", hal->type());

    _hal = hal;

    return true;
}

void HAL::Destroy()
{
    if (_hal == nullptr)
    {
        spdlog::error("HAL not exist");
        return;
    }

    delete _hal;
    _hal = nullptr;
}

void HAL::renderFpsPanel()
{
    static unsigned long time_count = 0;

    _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    _canvas->setTextSize(1);
    _canvas->setTextDatum(top_left);
    _canvas->drawNumber(1000 / (millis() - time_count), 0, 0, &fonts::Font0);
    // _canvas->drawNumber(123, 0, 0, &fonts::Font0);

    time_count = millis();
}

static constexpr int _msg_start_x = 16;
static constexpr int _msg_start_y = 130;
static constexpr int _msg_line_height = 32;

void HAL::popFatalError(std::string msg)
{
    // Echo
    spdlog::error("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopFatalError);
    _canvas->setBaseColor(AssetPool::GetColor().Misc.bgPopFatalError);
    _canvas->fillScreen(AssetPool::GetColor().Misc.bgPopFatalError);

    _canvas->setTextSize(1);
    _canvas->setTextDatum(top_left);
    _canvas->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
    _canvas->setCursor(20, 10);
    _canvas->printf(":(");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont24(_canvas);
    _canvas->setCursor(_msg_start_x, line_height);
    _canvas->printf("Fatal Error!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line))
    {
        line_height += _msg_line_height;
        _canvas->setCursor(_msg_start_x, line_height);
        _canvas->print(line.c_str());
    }

    /* ---------------------------------- Push ---------------------------------- */
    _canvas->pushSprite(0, 0);
    _display->setBrightness(255);

    // Press any button to poweroff
    while (1)
    {
        feedTheDog();
        delay(50);

        if (getAnyButton())
            reboot();
    }
}

void HAL::popWarning(std::string msg)
{
    // Echo
    spdlog::warn("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopWarning);
    _canvas->setBaseColor(AssetPool::GetColor().Misc.bgPopWarning);
    _canvas->fillScreen(AssetPool::GetColor().Misc.bgPopWarning);

    _canvas->setTextSize(1);
    _canvas->setTextDatum(top_left);
    _canvas->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
    _canvas->setCursor(15, 15);
    _canvas->printf("o.O?");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont24(_canvas);
    _canvas->setCursor(_msg_start_x, line_height);
    _canvas->printf("Warning!");

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line))
    {
        line_height += _msg_line_height;
        _canvas->setCursor(_msg_start_x, line_height);
        _canvas->print(line.c_str());
    }

    /* ---------------------------------- Push ---------------------------------- */
    _canvas->pushSprite(0, 0);
    // _display->setBrightness(255);

    // Press any button to poweroff
    while (1)
    {
        feedTheDog();
        delay(50);

        if (getAnyButton())
        {
            while (getAnyButton())
            {
                feedTheDog();
                delay(50);
            }
            break;
        }
    }
}

void HAL::popSuccess(std::string msg, bool showSuccessLabel)
{
    // Echo
    spdlog::info("{}", msg);

    /* ---------------------------------- Face ---------------------------------- */
    _canvas->setTextColor(TFT_WHITE, AssetPool::GetColor().Misc.bgPopSuccess);
    _canvas->setBaseColor(AssetPool::GetColor().Misc.bgPopSuccess);
    _canvas->fillScreen(AssetPool::GetColor().Misc.bgPopSuccess);

    _canvas->setTextSize(1);
    _canvas->setTextDatum(top_left);
    _canvas->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_72);
    _canvas->setCursor(20, 10);
    _canvas->printf(":)");

    /* ----------------------------------- Msg ---------------------------------- */
    int line_height = _msg_start_y;
    AssetPool::LoadFont24(_canvas);
    if (showSuccessLabel)
    {
        _canvas->setCursor(_msg_start_x, line_height);
        _canvas->printf("Success!");
    }

    std::istringstream msg_stream(msg);
    std::string line;
    while (std::getline(msg_stream, line))
    {
        line_height += _msg_line_height;
        _canvas->setCursor(_msg_start_x, line_height);
        _canvas->print(line.c_str());
    }

    /* ---------------------------------- Push ---------------------------------- */
    _canvas->pushSprite(0, 0);
    // _display->setBrightness(255);

    // Press any button to poweroff
    while (1)
    {
        feedTheDog();
        delay(50);

        if (getAnyButton())
        {
            while (getAnyButton())
            {
                feedTheDog();
                delay(50);
            }
            break;
        }
    }
}

// Cpp sucks
tm* HAL::getLocalTime()
{
    time(&_time_buffer);
    return localtime(&_time_buffer);
}

bool HAL::getAnyButton()
{
    for (int i = GAMEPAD::BTN_START; i < GAMEPAD::GAMEPAD_BUTTON_NUM; i++)
    {
        if (getButton(static_cast<GAMEPAD::GamePadButton_t>(i)))
            return true;
    }
    return false;
}

bool HAL::checkWifiConfig()
{
    if (_config.wifiSsid.empty())
    {
        popWarning("WiFi SSID\nIs empty");
        return false;
    }

    if (_config.wifiPassword.empty())
    {
        popWarning("WiFi Password\nIs empty");
        return false;
    }

    return true;
}

POWER_MONITOR::UnitAdaptatedData_t HAL::GetUnitAdaptatedVoltage(const float& voltage)
{
    POWER_MONITOR::UnitAdaptatedData_t data;

    if (std::abs(voltage) >= 1.0)
    {
        data.value = spdlog::fmt_lib::format("{:.2f}", voltage);
        data.unit = "V";
    }
    else if (std::abs(voltage) >= 0.001)
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", voltage * 1000.0);
        data.unit = "mV";
    }
    else
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", voltage * 1000000.0);
        // data.unit = "μV";
        data.unit = "mV";
    }

    return data;
}

POWER_MONITOR::UnitAdaptatedData_t HAL::GetUnitAdaptatedCurrent(const float& current)
{
    POWER_MONITOR::UnitAdaptatedData_t data;

    if (std::abs(current) >= 1.0)
    {
        data.value = spdlog::fmt_lib::format("{:.3f}", current);
        data.unit = "A";
    }
    else if (std::abs(current) >= 0.001)
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", current * 1000.0);
        data.unit = "mA";
    }
    else
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", current * 1000000.0);
        data.unit = "μA";
    }

    return data;
}

POWER_MONITOR::UnitAdaptatedData_t HAL::GetUnitAdaptatedPower(const float& power)
{
    POWER_MONITOR::UnitAdaptatedData_t data;

    if (std::abs(power) >= 1.0)
    {
        data.value = spdlog::fmt_lib::format("{:.2f}", power);
        data.unit = "W";
    }
    else if (std::abs(power) >= 0.001)
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", power * 1000.0);
        data.unit = "mW";
    }
    else
    {
        data.value = spdlog::fmt_lib::format("{:.0f}", power * 1000000.0);
        data.unit = "μW";
    }

    return data;
}

POWER_MONITOR::UnitAdaptatedData_t HAL::GetUnitAdaptatedCapacity(const float& capacity)
{
    POWER_MONITOR::UnitAdaptatedData_t data;

    if (std::abs(capacity) >= 1.0)
    {
        data.value = spdlog::fmt_lib::format("{:.3f}", capacity);
        data.unit = "Ah";
    }
    else if (std::abs(capacity) >= 0.001)
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", capacity * 1000.0);
        data.unit = "mAh";
    }
    else
    {
        data.value = spdlog::fmt_lib::format("{:.0f}", capacity * 1000000.0);
        data.unit = "μAh";
    }

    return data;
}

POWER_MONITOR::UnitAdaptatedData_t HAL::GetUnitAdaptatedEnergy(const float& energy)
{
    POWER_MONITOR::UnitAdaptatedData_t data;

    if (std::abs(energy) >= 1.0)
    {
        data.value = spdlog::fmt_lib::format("{:.3f}", energy);
        data.unit = "Wh";
    }
    else if (std::abs(energy) >= 0.001)
    {
        data.value = spdlog::fmt_lib::format("{:.1f}", energy * 1000.0);
        data.unit = "mWh";
    }
    else
    {
        data.value = spdlog::fmt_lib::format("{:.0f}", energy * 1000000.0);
        data.unit = "μWh";
    }

    return data;
}
