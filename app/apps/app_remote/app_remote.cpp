/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "app_remote.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../../assets/assets.h"

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;

// Theme color
void* AppRemote_Packer::getCustomData() { return (void*)(&AssetPool::GetStaticAsset()->Color.AppRemote.background); }

// Icon
void* AppRemote_Packer::getAppIcon() { return (void*)AssetPool::GetStaticAsset()->Image.AppRemote.app_icon; }

// Like setup()...
void AppRemote::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // Render full screen to theme color
    HAL::GetCanvas()->fillScreen(AssetPool::GetStaticAsset()->Color.AppRemote.background);
    HAL::CanvasUpdate();
}

// Like loop()...
void AppRemote::onRunning()
{
    // Print hi in every 1s
    if (HAL::Millis() - _data.time_count > 1000)
    {
        spdlog::info("Hi");
        _data.time_count = HAL::Millis();
    }

    // Update button state
    Button::Update();

    // Handle encoder button click
    if (Button::Encoder()->wasClicked())
        spdlog::info("encoder button clicked");

    // Check quit
    if (Button::Side()->isHolding())
        destroyApp();

    // ...
    // If you want to block here, feed the dog manually
    // HAL::FeedTheDog();
}

void AppRemote::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
