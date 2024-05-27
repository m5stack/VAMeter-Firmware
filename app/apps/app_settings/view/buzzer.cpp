/**
 * @file buzzer.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-23
 *
 * @copyright Copyright (c) 2024
 *
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

// Page: /buzzer
void AppSettings::_on_page_buzzer()
{
    spdlog::info("on page buzzer");

    auto history_beepon = HAL::GetSystemConfig().beepOn;

    int selected_index = HAL::GetSystemConfig().beepOn ? 0 : 1;
    while (1)
    {
        // std::vector<std::string> options = {" - ON", " - OFF", " - Back"};
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppSettings_Option_On);
        options.push_back(AssetPool::GetText().AppSettings_Option_Off);
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(
            AssetPool::GetText().AppSettings_Option_Buzzer, options, selected_index, &_data.select_page_theme);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        else if (selected_index == 0)
            HAL::GetSystemConfig().beepOn = true;
        else if (selected_index == 1)
            HAL::GetSystemConfig().beepOn = false;

        break;
    }

    // Check save
    if (history_beepon != HAL::GetSystemConfig().beepOn)
    {
        HAL::SaveSystemConfig();
    }
}
