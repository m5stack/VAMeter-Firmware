/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include "../../../../hal/hal.h"
#include "../../triggers/triggers.h"
#include <smooth_ui_toolkit.h>

class ConfigSelector : public SmoothUIToolKit::Widgets::Selector::SmoothSelector
{
private:
public:
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onRender() override;
    void onUpdate() override;
};

class ConfigPanel : public SmoothUIToolKit::Widgets::SmoothWidgetBase
{
public:
    struct Config_t
    {
        uint32_t record_time = 10000;
        TriggerMode_t trigger_mode = trigger_mode_manual;
        float threshold = 1.0f;
    };

private:
    struct Data_t
    {
        ConfigSelector* selector = nullptr;
        Config_t config;
    };
    Data_t _data;

public:
    ~ConfigPanel();
    void onInit() override;
    void popOut() override;
    void onPopOut() override;
    void hide() override;
    void onHide() override;
    void onRender() override;
    void onPostRender() override;
    void onUpdate() override;

    inline const Config_t& getConfig() { return _data.config; }
};

class ConfigReacordTime : public SmoothUIToolKit::Widgets::Selector::SmoothOption
{
private:
    ConfigPanel::Config_t* _config;

public:
    ConfigReacordTime(ConfigPanel::Config_t* config) : _config(config) {}
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
    void onSelect() override;
    void onUnSelect() override;
};

class ConfigTrigerMode : public SmoothUIToolKit::Widgets::Selector::SmoothOption
{
private:
    ConfigPanel::Config_t* _config;
    std::vector<std::string> _trigger_mode_label_list;

public:
    ConfigTrigerMode(ConfigPanel::Config_t* config) : _config(config) {}
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
};

class ConfigThreshold : public SmoothUIToolKit::Widgets::Selector::SmoothOption
{
private:
    ConfigPanel::Config_t* _config;

public:
    ConfigThreshold(ConfigPanel::Config_t* config) : _config(config) {}
    void onInit() override;
    void onPopOut() override;
    void onHide() override;
    void onUpdate() override;
    void onRender() override;
};
