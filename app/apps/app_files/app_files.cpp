/**
 * @file app_files.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_files.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../../assets/assets.h"
#include <string>
#include <vector>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::UI;
using namespace SYSTEM::INPUTS;

const char* AppFiles_Packer::getAppName() { return AssetPool::GetText().AppName_Files; }

// Theme color
void* AppFiles_Packer::getCustomData() { return (void*)(&AssetPool::GetColor().AppFile.selector); }

// Icon
void* AppFiles_Packer::getAppIcon() { return (void*)AssetPool::GetImage().AppFile.app_icon; }

// Like setup()...
void AppFiles::onResume() { spdlog::info("{} onResume", getAppName()); }

// Like loop()...
void AppFiles::onRunning()
{
    _on_page_root();
    destroyApp();
}

void AppFiles::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
