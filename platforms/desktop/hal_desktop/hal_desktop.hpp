/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "spdlog/spdlog.h"
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include "lgfx/v1/LGFX_Sprite.hpp"
#include "lgfx/v1/platforms/sdl/common.hpp"
#include <hal/hal.h>
#include "lgfx/v1/lgfx_fonts.hpp"
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>
#include <random>
#include <assets/assets.h>

class HAL_Desktop : public HAL
{
private:
    int _screenWidth;
    int _screenHeight;

public:
    std::string type() override { return "Desktop"; }

    HAL_Desktop(int screenWidth = 240, int screenHeight = 240)
    {
        _screenWidth = screenWidth;
        _screenHeight = screenHeight;
    }

    void init() override
    {
        // Display
        _display = new LGFX(_screenWidth, _screenHeight);
        _display->init();

        // Canvas
        _canvas = new LGFX_SpriteFx(_display);
        _canvas->createSprite(_display->width(), _display->height());

        // Some pc window pop up slower?
        lgfx::delay(1500);

        // this->popWarning("404 not found\nasdasd asdfasf");
        // this->popSuccess("404 not found\nasdasd asdfasf");
        // this->popFatalError("404 not found\nasdasd asdfasf");

        _config.wifiSsid = "114514";
        _config.wifiPassword = "1919810";
    }

    void canvasUpdate() override { GetCanvas()->pushSprite(0, 0); }

    bool getButton(GAMEPAD::GamePadButton_t button) override
    {
        if (button == GAMEPAD::BTN_A)
            return !lgfx::gpio_in(36);
        // else if (button == GAMEPAD::BTN_LEFT)
        //     return !lgfx::gpio_in(39);
        // else if (button == GAMEPAD::BTN_RIGHT)
        //     return !lgfx::gpio_in(37);
        else if (button == GAMEPAD::BTN_B)
            return !lgfx::gpio_in(38);
        // else if (button == GAMEPAD::BTN_SELECT)
        //     return !lgfx::gpio_in(38);
        // else if (button == GAMEPAD::BTN_UP)
        //     return !lgfx::gpio_in(39);
        // else if (button == GAMEPAD::BTN_DOWN)
        //     return !lgfx::gpio_in(37);
        else if (button == GAMEPAD::BTN_START)
            return !lgfx::gpio_in(38);

        return false;
    }

    // Simulate encoder by mouse drag
    int encoder_count = 0;
    int last_encoder_count = 0;
    int touch_start_x = 0;
    int touch_last_x = 0;
    bool is_touching = false;
    int getEncoderCount() override
    {
        if (isTouching())
        {
            if (!is_touching)
            {
                is_touching = true;
                touch_start_x = getTouchPoint().x;
                return encoder_count;
            }

            if (getTouchPoint().x != touch_last_x)
            {
                // spdlog::info("{} {} {}", touch_start_x, getTouchPoint().x, encoder_count);
                encoder_count = (getTouchPoint().x - touch_start_x) / 20 + last_encoder_count;
            }
            touch_last_x = getTouchPoint().x;
        }
        else if (is_touching)
        {
            is_touching = false;
            last_encoder_count = encoder_count;
        }

        // // Reverse
        // return -encoder_count;
        // // return encoder_count;

        if (_config.reverseEncoder)
            return encoder_count;
        return -encoder_count;
    }

    void resetEncoderCount(int value) override { encoder_count = value; }

    bool isTouching() override
    {
        lgfx::touch_point_t tp;
        _display->getTouch(&tp);
        // spdlog::info("{} {} {}", tp.x, tp.y, tp.size);
        return tp.size != 0;
    }

    TOUCH::Point_t getTouchPoint() override
    {
        lgfx::touch_point_t tp;
        _display->getTouch(&tp);
        return {tp.x, tp.y};
    }

    float generateRandomNumber(float v1, float v2)
    {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_real_distribution<float> distribution(v1, v2);
        return distribution(generator);
    }

    // Generate fake data
    float sine_x = 0.0f;
    uint32_t time_count = 0;
    void updatePowerMonitor() override
    {
        _pm_data.busPower = generateRandomNumber(4.7, 5.3);

        // _pm_data.busVoltage = generateRandomNumber(4.7, 5.3);
        _pm_data.busVoltage = generateRandomNumber(4, 5.5);
        // _pm_data.busVoltage = generateRandomNumber(10, 20);
        // _pm_data.busVoltage = generateRandomNumber(0.01, 0.02);
        // _pm_data.busVoltage = 0.00125;
        // _pm_data.busVoltage = 0;

        // _pm_data.shuntCurrent = 0.1 * std::sin(sine_x);
        // _pm_data.shuntCurrent = std::abs(std::sin(sine_x));
        _pm_data.shuntCurrent = 0.1 * std::sin(sine_x) + 1 * 0.1;
        _pm_data.shuntCurrent = 0.1 * std::sin(sine_x) - 1 * 0.1;
        // _pm_data.shuntCurrent = (0.1 * std::sin(sine_x) + 1 * 0.1) * 10;
        // _pm_data.shuntCurrent = 0.0001 * std::sin(sine_x) + 1 * 0.001;
        // _pm_data.shuntCurrent = 0.000066;

        _pm_data.shuntVoltage = std::cos(sine_x);
        // _pm_data.shuntVoltage = 0;
        // _pm_data.shuntVoltage = 0.00125;

        sine_x += 0.2;

        // Fake daemon data
        _pm_data.currentAvgSecond = generateRandomNumber(0.0, 1.2);
        // _pm_data.currentAvgSecond = 0.5;

        _pm_data.currentAvgMin = generateRandomNumber(0.0, 2.2);
        _pm_data.currentAvgTotal = generateRandomNumber(0.0, 3.2);
        _pm_data.currentPeak = generateRandomNumber(0.0, 3.2);
        _pm_data.currentMin = generateRandomNumber(0.0, 3.2);

        _pm_data.capacity += 0.000222;
        _pm_data.capacity = 0.00022332;

        _pm_data.energy += 0.02222;

        _pm_data.time = millis() - time_count;
    }

    void resetPowerMonitorData() override
    {
        _pm_data.capacity = 0.0f;
        _pm_data.energy = 0.0f;
        time_count = millis();
    }

    std::vector<VA_RECORDER::RecordData_t> getVaRecord(const std::string& recordName) override
    {
        std::vector<VA_RECORDER::RecordData_t> fake_data;

        float sine_x = 0.0f;
        // for (int i = 0; i < 120; i++)
        for (int i = 0; i < 240 * 2; i++)
        {
            // fake_data.push_back(VA_RECORDER::RecordData_t(generateRandomNumber(4, 5.5), 2));

            fake_data.push_back(VA_RECORDER::RecordData_t(generateRandomNumber(4, 5.5), 0.1 * std::sin(sine_x) + 1 * 0.1));
            sine_x += 0.2;
        }

        // fake_data.resize(70);

        return fake_data;
    }

    std::vector<std::string> getVaRecordNameList() override
    {
        std::vector<std::string> list = {
            "REC-001.csv",
            "REC-002.csv",
            "REC-003.csv",
            "REC-004.csv",
            "REC-005.csv",
            "REC-006.csv",
            "REC-007.csv",
        };
        return list;
    };

    bool connectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect) override
    {
        onLogPageRender(" connecting wifi..\n", true, true);
        delay(300);
        return true;
    }

    bool uploadVaRecordViaEzData(const std::string& recordName, OnLogPageRenderCallback_t onLogPageRender) override
    {
        onLogPageRender(" upload via ezdata..\n", true, true);
        delay(300);

        onLogPageRender(" open: ", false, false);
        onLogPageRender(recordName, false, false);
        onLogPageRender("\n", true, false);
        delay(300);

        onLogPageRender(" uploading..\n", true, false);
        delay(300);

        onLogPageRender(" done\n", true, false);
        delay(1000);

        return true;
    }

    OTA_UPGRADE::OtaInfo_t getLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender) override
    {
        OTA_UPGRADE::OtaInfo_t info;
        info.firmwareUrl = "www.114514.com";
        info.latestVersion = "V6.6.6";
        info.upgradeAvailable = true;
        info.needUsbFlashing = true;
        info.description = " - Fix ssss saf\n Fix hhigeroh sdg &^*\n - Add ssho\n - Better asdff noff??";
        return info;
    }

    bool upgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl) override
    {
        onLogPageRender(" upload via ezdata..\n", true, true);
        delay(200);

        onLogPageRender(" connecting wifi..\n", true, false);
        delay(200);

        onLogPageRender(" upgrading..\n", true, false);
        delay(200);

        for (int i = 0; i < 100; i += 5)
        {
            std::string log = "<PB>";
            log += std::to_string(i);
            log += "\n";
            onLogPageRender(log, true, false);
            delay(100);
        }

        onLogPageRender(" done\n", true, true);
        delay(200);

        popSuccess(AssetPool::GetText().Misc_Text_UpgradeDone);

        return true;
    }

    bool renderCustomStartupImage() override
    {
        return _canvas->drawJpgFile("startup_image.jpeg", 0, 0, _canvas->width(), _canvas->height());
    }

    std::vector<std::string> getStartupImageList() override
    {
        std::vector<std::string> ret = {"Nihao.jpeg", "jjj.png", "888**..."};
        return ret;
    }

    // std::string getSystemConfigUrl() override { return "http://127.0.0.1/syscfg"; }
};
