/**
 * @file app_settings.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_settings.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../../assets/assets.h"
#include "apps/utils/system/ui/misc/misc.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <string>
#include <vector>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::UI;

const char* AppSettings_Packer::getAppName() { return AssetPool::GetText().AppName_Settings; }

// Theme color
void* AppSettings_Packer::getCustomData() { return (void*)(&AssetPool::GetColor().AppSettings.selector); }

// Icon
void* AppSettings_Packer::getAppIcon() { return (void*)AssetPool::GetStaticAsset()->Image.AppSettings.app_icon; }

// Like setup()...
void AppSettings::onResume()
{
    spdlog::info("{} onResume", getAppName());

    _data.select_page_theme.background = AssetPool::GetColor().AppSettings.background;
    _data.select_page_theme.optionText = AssetPool::GetColor().AppSettings.optionText;
    _data.select_page_theme.selector = AssetPool::GetColor().AppSettings.selector;
}

// Like loop()...
void AppSettings::onRunning()
{
    _on_page_root();
    destroyApp();
}

void AppSettings::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
