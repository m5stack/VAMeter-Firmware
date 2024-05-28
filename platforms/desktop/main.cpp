/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
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
