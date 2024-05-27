/**
 * @file startup_image.cpp
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
#include "hal/types.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /startup image
void AppSettings::_on_page_startup_image()
{
    spdlog::info("on page startup image");

    auto image_list = HAL::GetStartupImageList();

    // image_list.push_back(AssetPool::GetText().AppSettings_Option_Default);
    image_list.push_back("Default Dark");
    image_list.push_back("Default Light");
    image_list.push_back(AssetPool::GetText().AppSettings_Option_Off);
    image_list.push_back(AssetPool::GetText().AppSettings_Option_Back);

    auto selected_index = 0;
    selected_index = SelectMenuPage::CreateAndWaitResult(
        AssetPool::GetText().AppSettings_Option_StartupImage, image_list, selected_index, &_data.select_page_theme);

    // Back
    if (selected_index == -1)
        return;
    if (selected_index == image_list.size() - 1)
        return;

    // Store config
    if (selected_index == image_list.size() - 2)
        HAL::GetSystemConfig().startupImage = PASS_STARTUP_IMAGE_TAG;
    else if (selected_index == image_list.size() - 3)
        HAL::GetSystemConfig().startupImage = DEFAULT_STARTUP_IMAGE_LIGHT_TAG;
    else if (selected_index == image_list.size() - 4)
        HAL::GetSystemConfig().startupImage = DEFAULT_STARTUP_IMAGE_DARK_TAG;
    else
        HAL::GetSystemConfig().startupImage = image_list[selected_index];

    spdlog::info("select: {}", HAL::GetSystemConfig().startupImage);
    // std::vector<std::string>().swap(image_list);

    if (CreateConfirmPage(AssetPool::GetText().Misc_RebootNow, true, &_data.select_page_theme))
    {
        HAL::SaveSystemConfig();
        HAL::Reboot();
    }
}
