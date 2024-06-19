/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_power_monitor.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../../assets/assets.h"
#include "spdlog/spdlog.h"
#include "view/view.h"
#include <mooncake.h>
#include <string>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace VIEWS;

const char* AppPower_monitor_Packer::getAppName() { return AssetPool::GetText().AppName_PowerMonitor; }

// Theme color
// void* AppPower_monitor_Packer::getCustomData() { return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.primary);
// }
void* AppPower_monitor_Packer::getCustomData()
{
    auto app_history = HAL::NvsGet(NVS_KEY_APP_HISTORY);
    switch (app_history)
    {
    case 0:
        return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.primary);
    case 1:
        return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageBusVoltage);
    case 2:
        return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageShuntCurrent);
    case 3:
        return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageBusPower);
    case 4:
        return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageMoreDetailBackground);
    default:
        break;
    }
    spdlog::warn("no theme color on history {}", app_history);
    return (void*)(&AssetPool::GetStaticAsset()->Color.AppPowerMonitor.primary);
}

// Icon
void* AppPower_monitor_Packer::getAppIcon() { return (void*)AssetPool::GetStaticAsset()->Image.AppPowerMonitor.app_icon; }

// Page 0~4
constexpr static int _total_page_num = 4;

// Like setup()...
void AppPower_monitor::onResume()
{
    spdlog::info("{} onResume", getAppName());
    _data.view = new PmDataPage;
    _data.is_page_switched = true;

    // Load history page
    _data.current_page_num = HAL::NvsGet(NVS_KEY_APP_HISTORY);
    if (_data.current_page_num > _total_page_num || _data.current_page_num < 0)
    {
        spdlog::error("wrong history page {}", _data.current_page_num);
        _data.current_page_num = 0;
    }

    Encoder::Reset();
}

// Like loop()...
void AppPower_monitor::onRunning()
{
    _check_page_switch();
    _update_view();
}

void AppPower_monitor::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    delete _data.view;
}

void AppPower_monitor::_check_page_switch()
{
    // Scroll encoder to switch page
    Encoder::Update();
    if (Encoder::WasMoved())
    {
        if (Encoder::GetDirection() < 0)
        {
            if (_data.current_page_num >= _total_page_num)
                return;
            _data.current_page_num++;
            _data.is_page_switched = true;
        }
        else
        {
            if (_data.current_page_num <= 0)
                return;
            _data.current_page_num--;
            _data.is_page_switched = true;
        }
    }

    // Update page
    if (_data.is_page_switched)
    {
        _data.is_page_switched = false;

        //  Setup to new page
        spdlog::info("switch to page {}", _data.current_page_num);
        switch (_data.current_page_num)
        {
        case 0:
            _setup_page_simple_detail();
            HAL::NvsSet(NVS_KEY_APP_HISTORY, 0);
            break;
        case 1:
            _data.view->quitSimpleDetailPage();
            _setup_page_bus_volt();
            _data.view->reset();
            HAL::NvsSet(NVS_KEY_APP_HISTORY, 1);
            break;
        case 2:
            _setup_page_shunt_current();
            _data.view->reset();
            HAL::NvsSet(NVS_KEY_APP_HISTORY, 2);
            break;
        case 3:
            _data.view->quitMoreDetailPage();
            _setup_page_bus_power();
            _data.view->reset();
            HAL::NvsSet(NVS_KEY_APP_HISTORY, 3);
            break;
        // case 4:
        //     _data.view->quitMoreDetailPage();
        //     _setup_page_shunt_volt();
        //     _data.view->reset();
        //     break;
        case 4:
            _setup_page_more_detail();
            HAL::NvsSet(NVS_KEY_APP_HISTORY, 4);
            break;
        default:
            break;
        }
    }
}

void AppPower_monitor::_update_view()
{
    if (HAL::Millis() - _data.pm_update_time_count > 100)
    {
        HAL::UpdatePowerMonitor();
        _data.pm_update_time_count = HAL::Millis();
    }

    _data.view->update(HAL::Millis());

    if (_data.view->want2quit())
        destroyApp();
}

void AppPower_monitor::_setup_page_bus_volt()
{
    spdlog::info("page bus volt");

    _data.view->setConfig().themeColor = AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageBusVoltage;
    _data.view->setConfig().showLcModeLabel = false;
    _data.view->setConfig().panelImage = (void*)(AssetPool::GetStaticAsset()->Image.AppPowerMonitor.page_bus_volt_panel);

    _data.view->setConfig().getValueCallback = [](std::string& text) {
        text = HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().busVoltage).value;
    };

    _data.view->setConfig().getLabelCallback = [](std::string& text) {
        text = spdlog::fmt_lib::format("{} ({})",
                                       AssetPool::GetText().AppPowerMonitor_PageSingle_InputVolt,
                                       HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().busVoltage).unit);
    };
}

void AppPower_monitor::_setup_page_bus_power()
{
    spdlog::info("page bus power");

    _data.view->setConfig().themeColor = AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageBusPower;
    _data.view->setConfig().showLcModeLabel = false;
    _data.view->setConfig().panelImage = (void*)(AssetPool::GetStaticAsset()->Image.AppPowerMonitor.page_bus_power_panel);

    _data.view->setConfig().getValueCallback = [](std::string& text) {
        text = HAL::GetUnitAdaptatedPower(HAL::GetPowerMonitorData().busPower).value;
    };

    _data.view->setConfig().getLabelCallback = [](std::string& text) {
        text = spdlog::fmt_lib::format("{} ({})",
                                       AssetPool::GetText().AppPowerMonitor_PageSingle_InputPower,
                                       HAL::GetUnitAdaptatedPower(HAL::GetPowerMonitorData().busPower).unit);
    };
}

void AppPower_monitor::_setup_page_shunt_current()
{
    spdlog::info("page shunt current");

    _data.view->setConfig().themeColor = AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageShuntCurrent;
    _data.view->setConfig().showLcModeLabel = true;
    _data.view->setConfig().panelImage = (void*)(AssetPool::GetStaticAsset()->Image.AppPowerMonitor.page_shunt_current_panel);

    _data.view->setConfig().getValueCallback = [](std::string& text) {
        text = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().shuntCurrent).value;
    };

    _data.view->setConfig().getLabelCallback = [](std::string& text) {
        text = spdlog::fmt_lib::format("{} ({})",
                                       AssetPool::GetText().AppPowerMonitor_PageSingle_OutputCurrent,
                                       HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().shuntCurrent).unit);
    };
}

void AppPower_monitor::_setup_page_shunt_volt()
{
    spdlog::info("page shunt volt");

    _data.view->setConfig().themeColor = AssetPool::GetStaticAsset()->Color.AppPowerMonitor.pageShuntVoltage;
    _data.view->setConfig().showLcModeLabel = true;
    _data.view->setConfig().panelImage = (void*)(AssetPool::GetStaticAsset()->Image.AppPowerMonitor.page_shunt_volt_panel);

    _data.view->setConfig().getValueCallback = [](std::string& text) {
        text = HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().shuntVoltage).value;
    };

    _data.view->setConfig().getLabelCallback = [](std::string& text) {
        text = spdlog::fmt_lib::format("{} ({})",
                                       AssetPool::GetText().AppPowerMonitor_PageSingle_ShuntVolt,
                                       HAL::GetUnitAdaptatedVoltage(HAL::GetPowerMonitorData().shuntVoltage).unit);
    };
}

void AppPower_monitor::_setup_page_simple_detail()
{
    spdlog::info("page simple detail");
    _data.view->setConfig().themeColor = AssetPool::GetStaticAsset()->Color.AppPowerMonitor.primary;
    _data.view->reset();
    _data.view->goSimpleDetailPage();
}

void AppPower_monitor::_setup_page_more_detail()
{
    spdlog::info("page all detail");
    _data.view->goMoreDetailPage();
}
