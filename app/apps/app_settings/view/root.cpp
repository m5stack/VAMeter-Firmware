/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_settings.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "../../app_startup_anim/app_startup_anim.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /
void AppSettings::_on_page_root()
{
    spdlog::info("on page root");

    int selected_index = 0;
    while (1)
    {
        std::vector<std::string> options;
        options.push_back("Operation Guide");
        options.push_back(AssetPool::GetText().AppSettings_Option_About);
        options.push_back(AssetPool::GetText().AppSettings_Option_Display);
        options.push_back(AssetPool::GetText().AppSettings_Option_Buzzer);
        options.push_back(AssetPool::GetText().AppSettings_Option_Encoder);
        options.push_back(AssetPool::GetText().AppSettings_Option_Calibration);
        options.push_back(AssetPool::GetText().AppSettings_Option_Network);
        options.push_back(AssetPool::GetText().AppSettings_Option_OTA);
        options.push_back(AssetPool::GetText().AppSettings_Option_Language);
        options.push_back(AssetPool::GetText().AppSettings_Option_StartupImage);
        options.push_back("Base Test");
        options.push_back("Factory Reset");
        options.push_back(AssetPool::GetText().AppSettings_Option_Quit);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppName_Settings, options, selected_index, &_data.select_page_theme);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        else if (selected_index == 0)
            AppStartupAnim::PopUpGuideMap(true);

        else if (selected_index == 1)
            _on_page_about();

        else if (selected_index == 2)
            _on_page_display();

        else if (selected_index == 3)
            _on_page_buzzer();

        else if (selected_index == 4)
            _on_page_encoder();

        else if (selected_index == 5)
            _on_page_calibration();

        else if (selected_index == 6)
            _on_page_network();

        else if (selected_index == 7)
            _on_page_ota_upgrade();

        else if (selected_index == 8)
            _on_page_language();

        else if (selected_index == 9)
            _on_page_startup_image();

        else if (selected_index == 10)
            _on_page_base_test();

        else if (selected_index == 11)
            _on_factory_reset();
    }

    // // Save changes
    // if (_data.is_changed)
    // {
    //     if (CreateConfirmPage(AssetPool::GetText().AppSettings_Notice_Saving, true, &_data.select_page_theme))
    //         HAL::SaveSystemConfig();
    // }

    // if (_data.need_reboot)
    // {
    //     if (CreateConfirmPage(AssetPool::GetText().Misc_RebootNow, true, &_data.select_page_theme))
    //         HAL::Reboot();
    // }
}
