/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "view.h"
#include "../../../hal/hal.h"
#include "../../../assets/assets.h"
#include "../../utils/system/system.h"
#include <smooth_ui_toolkit.h>
#include <mooncake.h>
#include <cstdint>
// Refs:
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/format-specification-syntax-printf-and-wprintf-functions

using namespace VIEWS;
using namespace SmoothUIToolKit;
using namespace SYSTEM::INPUTS;

/* -------------------------------------------------------------------------- */
/*                               All detail page                              */
/* -------------------------------------------------------------------------- */
void PmDataPage::goMoreDetailPage()
{
    if (_data.is_all_detail_page)
        return;

    _data.is_all_detail_page = true;
    _data.bg_color.moveTo(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailBackground);

    // HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_16);
    AssetPool::LoadFont16(HAL::GetCanvas());

    // Add transitions
    _data.transition_list_buffer.resize(7);

    /* ---------------------------- Avg current in 1s --------------------------- */
    _data.transition_list_buffer[0].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillRect(transition->getValue().x,
                                   transition->getValue().y - 1,
                                   HAL::GetCanvas()->width(),
                                   34 + 2,
                                   AssetPool::GetColor().AppPowerMonitor.pageMoreDetailPanel);

        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        // HAL::GetCanvas()->drawString("Avg. (1s)", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_PerS,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailAvgHightLight);
        // HAL::GetCanvas()->drawString("Avg.", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_HighLight,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgSecond).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgSecond).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* --------------------------- Avg current in 1min -------------------------- */
    _data.transition_list_buffer[1].setUpdateCallback([&](Transition2D* transition) {
        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        // HAL::GetCanvas()->drawString("Avg. (1min)", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_PerMin,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailAvgHightLight);
        // HAL::GetCanvas()->drawString("Avg.", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_HighLight,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgMin).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgMin).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* ---------------------------- Avg current total --------------------------- */
    _data.transition_list_buffer[2].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillRect(transition->getValue().x,
                                   transition->getValue().y,
                                   HAL::GetCanvas()->width(),
                                   34,
                                   AssetPool::GetColor().AppPowerMonitor.pageMoreDetailPanel);

        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        // HAL::GetCanvas()->drawString("Avg. (total)", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_Total,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailAvgHightLight);
        // HAL::GetCanvas()->drawString("Avg.", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Avg_HighLight,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgTotal).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentAvgTotal).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* ------------------------------ Peak current ------------------------------ */
    _data.transition_list_buffer[3].setUpdateCallback([&](Transition2D* transition) {
        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        // HAL::GetCanvas()->drawString("Curr. (Peak)", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Cur_Peak,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailCurrentHightLight);
        // HAL::GetCanvas()->drawString("Curr.", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Cur_HighLight,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentPeak).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentPeak).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* ------------------------------- Min current ------------------------------ */
    _data.transition_list_buffer[4].setUpdateCallback([&](Transition2D* transition) {
        HAL::GetCanvas()->fillRect(transition->getValue().x,
                                   transition->getValue().y,
                                   HAL::GetCanvas()->width(),
                                   34,
                                   AssetPool::GetColor().AppPowerMonitor.pageMoreDetailPanel);

        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        // HAL::GetCanvas()->drawString("Curr. (Min)", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Cur_Min,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailCurrentHightLight);
        // HAL::GetCanvas()->drawString("Curr.", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Cur_HighLight,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentMin).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCurrent(HAL::GetPowerMonitorData().currentMin).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* -------------------------------- Capacity -------------------------------- */
    _data.transition_list_buffer[5].setUpdateCallback([&](Transition2D* transition) {
        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailCapacityHightLight);
        // HAL::GetCanvas()->drawString("Capacity", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Capacity,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedCapacity(HAL::GetPowerMonitorData().capacity).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedCapacity(HAL::GetPowerMonitorData().capacity).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    /* --------------------------------- Energy --------------------------------- */
    _data.transition_list_buffer[6].setUpdateCallback([&](Transition2D* transition) {
        // By the way
        if (Button::Encoder()->wasClicked())
        {
            spdlog::info("reset pm data");
            HAL::ResetPowerMonitorData();
        }

        HAL::GetCanvas()->fillRect(transition->getValue().x,
                                   transition->getValue().y,
                                   HAL::GetCanvas()->width(),
                                   34 + 1,
                                   AssetPool::GetColor().AppPowerMonitor.pageMoreDetailPanel);

        // Name tag
        HAL::GetCanvas()->setTextDatum(middle_left);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailCapacityHightLight);
        // HAL::GetCanvas()->drawString("Energy", transition->getValue().x + 5, transition->getValue().y + 17);
        HAL::GetCanvas()->drawString(AssetPool::GetText().AppPowerMonitor_PageMoreDetail_Energy,
                                     transition->getValue().x + 5,
                                     transition->getValue().y + 17);

        // Value
        _data.string_buffer = HAL::GetUnitAdaptatedEnergy(HAL::GetPowerMonitorData().energy).value;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelValue);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 147, transition->getValue().y + 17);

        // Unit
        _data.string_buffer = HAL::GetUnitAdaptatedEnergy(HAL::GetPowerMonitorData().energy).unit;
        HAL::GetCanvas()->setTextDatum(middle_center);
        HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppPowerMonitor.pageMoreDetailLabelBasic);
        HAL::GetCanvas()->drawString(
            _data.string_buffer.c_str(), transition->getValue().x + 213, transition->getValue().y + 17);
    });

    // Transition config
    for (int i = 0; i < _data.transition_list_buffer.size(); i++)
    {
        _data.transition_list_buffer[i].jumpTo(-240, i * 34 + 1);
        _data.transition_list_buffer[i].moveTo(0, i * 34 + 1);
        _data.transition_list_buffer[i].setDelay(i * 50);
        _data.transition_list_buffer[i].setTransitionPath(EasingPath::easeOutBack);
        _data.transition_list_buffer[i].setDuration(400);
    }
}

void PmDataPage::quitMoreDetailPage()
{
    if (!_data.is_all_detail_page)
        return;

    // Free and reset
    _data.is_all_detail_page = false;
    _data.transition_list_buffer.clear();
    reset();
}
