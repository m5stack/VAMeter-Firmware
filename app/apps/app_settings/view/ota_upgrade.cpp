/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

void AppSettings::_on_page_ota_upgrade()
{
    spdlog::info("on page ota");

    if (HAL::ConnectWifi(OnOtaLogPageRender))
    {
        auto info = HAL::GetLatestFirmwareInfoViaOta(OnOtaLogPageRender);
        if (info.getInfoFailed)
        {
            HAL::PopWarning("Get info failed");
            return;
        }

        if (info.upgradeAvailable)
        {
            // Usb flashing notice
            if (info.needUsbFlashing)
            {
                CreateNoticePage("New Version", [&](Transition2D& transition) {
                    auto string_buffer =
                        spdlog::fmt_lib::format(" {} : {}", AssetPool::GetText().Misc_Text_LatestVersion, info.latestVersion);
                    HAL::GetCanvas()->print(string_buffer.c_str());

                    string_buffer = spdlog::fmt_lib::format("\n\n{}", AssetPool::GetText().Misc_Text_UsbFlashingNotice);
                    HAL::GetCanvas()->print(string_buffer.c_str());

                    AssetPool::LoadFont16(HAL::GetCanvas());
                    HAL::GetCanvas()->drawCenterString(
                        AssetPool::GetText().AppSettings_Option_Back, 120, 196 + transition.getValue().y);
                    HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
                });
                return;
            }

            // OK to upgrade
            if (CreateNoticePage("New Version", [&](Transition2D& transition) {
                    auto string_buffer =
                        spdlog::fmt_lib::format(" {} : {}", AssetPool::GetText().Misc_Text_LatestVersion, info.latestVersion);
                    HAL::GetCanvas()->print(string_buffer.c_str());

                    HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibold_16);
                    string_buffer = spdlog::fmt_lib::format("\n\n{}", info.description);
                    HAL::GetCanvas()->print(string_buffer.c_str());

                    AssetPool::LoadFont16(HAL::GetCanvas());
                    HAL::GetCanvas()->drawCenterString(
                        AssetPool::GetText().Misc_Text_ClickToUpgrade, 120, 196 + transition.getValue().y);
                    HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
                }))
            {
                HAL::UpgradeFirmwareViaOta(OnOtaLogPageRender, info.firmwareUrl);
            }
        }
        else
            HAL::PopSuccess(AssetPool::GetText().Misc_Text_IsAlreadyLatest, false);
    }
}
