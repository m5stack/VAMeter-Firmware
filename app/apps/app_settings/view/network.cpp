/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../utils/qrcode/qrcode.h"
#include "../../../assets/assets.h"
#include "spdlog/fmt/bundled/core.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;
using namespace QRCODE;

void AppSettings::_on_page_network()
{
    spdlog::info("on page network");

    std::string string_buffer;
    bool need_config = CreateNoticePage("Notice", [&](Transition2D& transition) {
        string_buffer = spdlog::fmt_lib::format(" WiFi Config:\n  [SSID]:\n   {}\n  [Password]:\n   {}",
                                                HAL::GetSystemConfig().wifiSsid,
                                                HAL::GetSystemConfig().wifiPassword);
        HAL::GetCanvas()->print(string_buffer.c_str());

        HAL::GetCanvas()->drawCenterString(AssetPool::GetText().Misc_Text_ClickToConfig, 120, 196 + transition.getValue().y);
        HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
    });

    if (need_config)
    {
        HAL::StartWebServer(OnLogPageRender);

        // Pop wifi code
        auto text = fmt::format("WIFI:T:nopass;S:{};;", HAL::GetApWifiSsid());
        std::vector<std::vector<bool>> qrcode_bitmap;
        GetQrcodeBitmap(qrcode_bitmap, text.c_str());

        CreateNoticePage(
            "WiFi Config",
            [&](Transition2D& transition) {
                RenderQRCodeBitmap(qrcode_bitmap,
                                   120 - 120 / 2,
                                   50 + transition.getValue().y,
                                   120,
                                   AssetPool::GetColor().AppSettings.optionText,
                                   AssetPool::GetColor().AppSettings.background);

                HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
                HAL::GetCanvas()->setTextDatum(middle_center);
                HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionTextLight);
                std::string string_buffer = fmt::format("----   {}   ----", HAL::GetApWifiSsid());
                HAL::GetCanvas()->drawString(string_buffer.c_str(), 120, 185 + transition.getValue().y);

                // Lable
                AssetPool::LoadFont24(HAL::GetCanvas());
                HAL::GetCanvas()->setTextDatum(middle_center);
                HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppSettings.optionText);
                HAL::GetCanvas()->drawString(AssetPool::GetText().Misc_Text_ConnectAp, 120, 215 + transition.getValue().y);
            },
            [&]() {
                HAL::FeedTheDog();
                HAL::Delay(50);

                Button::Update();
                if (Button::Encoder()->wasClicked())
                    return true;

                // Check sta num
                if (HAL::GetApStaNum() != 0)
                    return true;

                return false;
            });

        // Pop url code
        text = HAL::GetSystemConfigUrl();
        GetQrcodeBitmap(qrcode_bitmap, text.c_str());

        CreateNoticePage(
            "WiFi Config",
            [&](Transition2D& transition) {
                RenderQRCodeBitmap(qrcode_bitmap,
                                   120 - 120 / 2,
                                   40 + transition.getValue().y,
                                   120,
                                   AssetPool::GetColor().AppSettings.optionText,
                                   AssetPool::GetColor().AppSettings.background);

                HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
                HAL::GetCanvas()->setTextDatum(middle_center);
                HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionTextLight);
                HAL::GetCanvas()->drawString(text.c_str(), 120, 180 + transition.getValue().y);

                // Lable
                AssetPool::LoadFont24(HAL::GetCanvas());
                HAL::GetCanvas()->setTextDatum(middle_center);
                HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppSettings.optionText);
                HAL::GetCanvas()->drawString(AssetPool::GetText().Misc_Text_OpenLink, 120, 215 + transition.getValue().y);
            },
            nullptr);

        HAL::StopWebServer();
    }
}

// Old shit
// void AppSettings::_on_page_network()
// {
//     spdlog::info("on page network");

//     // HAL::LoadSystemConfig();

//     std::string string_buffer;
//     bool need_config = CreateNoticePage("Notice", [&](Transition2D& transition) {
//         string_buffer = spdlog::fmt_lib::format(" WiFi config:\n  - SSID:  {}\n  - Password: {}",
//                                                 HAL::GetSystemConfig().wifiSsid,
//                                                 HAL::GetSystemConfig().wifiPassword);
//         HAL::GetCanvas()->print(string_buffer.c_str());

//         HAL::GetCanvas()->drawCenterString(AssetPool::GetText().Misc_Text_ClickToConfig, 120, 196 + transition.getValue().y);
//         HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
//     });

//     if (need_config)
//     {
//         HAL::StartMscMode();

//         CreateNoticePage("MSC Mode", [&](Transition2D& transition) {
//             string_buffer = spdlog::fmt_lib::format(" 1. {}\n\n 2. {}",
//                                                     AssetPool::GetText().Misc_Text_ConnectTopUsb,
//                                                     AssetPool::GetText().Misc_Text_OpenNewUsbDrive);
//             HAL::GetCanvas()->print(string_buffer.c_str());

//             HAL::GetCanvas()->drawCenterString(
//                 AssetPool::GetText().Misc_Text_ClickToContinue, 120, 196 + transition.getValue().y);
//             HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
//         });

//         CreateNoticePage("Config", [&](Transition2D& transition) {
//             string_buffer = spdlog::fmt_lib::format(" 1. {} \"system_config.json\"\n\n 2. {}",
//                                                     AssetPool::GetText().AppFiles_Option_Open,
//                                                     AssetPool::GetText().Misc_Text_EditWiFiConfig);
//             HAL::GetCanvas()->print(string_buffer.c_str());

//             HAL::GetCanvas()->drawCenterString(
//                 AssetPool::GetText().Misc_Text_ClickToFinish, 120, 196 + transition.getValue().y);
//             HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
//         });

//         HAL::StopMscMode();

//         // // <Bug> dont work, json turn into trash
//         // // Reload config
//         // HAL::LoadSystemConfig();

//         SelectMenuPage::Theme_t theme;
//         theme.background = AssetPool::GetColor().AppSettings.background;
//         theme.optionText = AssetPool::GetColor().AppSettings.optionText;
//         theme.selector = AssetPool::GetColor().AppSettings.selector;
//         if (CreateConfirmPage(AssetPool::GetText().Misc_RebootNow, true, &theme))
//             HAL::Reboot();
//     }
// }
