/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>
#include <cmath>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

void AppSettings::_on_page_calibration()
{
    spdlog::info("on page calibration");

    if (!CreateNoticePage("Calibration", [](Transition2D& transition) {
            HAL::GetCanvas()->printf(
                "\n <-- %s\n     %s", AssetPool::GetText().Misc_Text_OnlyConnect, AssetPool::GetText().Misc_Text_ThisUsbPort);

            HAL::GetCanvas()->drawCenterString(
                AssetPool::GetText().Misc_Text_ClickToContinue, 120, 196 + transition.getValue().y);
            HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
        }))
        return;

    std::string string_buffer;
    std::vector<float> samples;
    OnLogPageRender(" > start sampling..\n", true, true);

    // Wait stable
    HAL::Delay(500);

    while (1)
    {
        HAL::UpdatePowerMonitor();

        // Sample, shunt voltage is raw and equals to raw current value
        samples.push_back(HAL::GetPowerMonitorData().shuntVoltage);

        string_buffer =
            spdlog::fmt_lib::format(" > S[{}] {:.7f} uA..\n", samples.size(), HAL::GetPowerMonitorData().shuntVoltage);
        OnLogPageRender(string_buffer, true, false);

        if (samples.size() >= 3000 / 40)
            break;

        HAL::Delay(40);
        HAL::FeedTheDog();
    }

    float sample_avg = 0.0f;
    for (const auto& i : samples)
        sample_avg += i;
    sample_avg = sample_avg / samples.size();
    spdlog::info("avg: {:.7f}", sample_avg);

    // One bit rounding up
    float current_offset = ceil(sample_avg * 1000000) / 1000000;

    // Store into config
    HAL::GetSystemConfig().currentOffset = current_offset;

    // Apply
    HAL::PowerMonitorCalibration(HAL::GetSystemConfig().currentOffset);

    spdlog::info("offset: {:.7f}", HAL::GetSystemConfig().currentOffset);

    HAL::PopSuccess(AssetPool::GetText().Misc_Text_CalibrationDone);

    HAL::SaveSystemConfig();
}
