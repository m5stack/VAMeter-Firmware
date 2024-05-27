/**
 * @file main.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <app.h>
#include "hal_desktop/hal_desktop.hpp"

void setup()
{
    APP::SetupCallback_t callback;

    callback.AssetPoolInjection = []() {
        AssetPool::InjectStaticAsset(AssetPool::CreateStaticAsset());
        // AssetPool::InjectStaticAsset(AssetPool::GetStaticAssetFromBin());
    };

    callback.HalInjection = []() { HAL::Inject(new HAL_Desktop(240, 240)); };

    APP::Setup(callback);
}

void loop() { APP::Loop(); }
