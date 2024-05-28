/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <mooncake.h>
#include <functional>
#include "assets/assets.h"
#include "hal/hal.h"

namespace APP
{
    struct SetupCallback_t
    {
        std::function<void()> AssetPoolInjection = nullptr;
        std::function<void()> HalInjection = nullptr;
    };

    void Setup(SetupCallback_t callback);
    void Loop();
    void Destroy();
} // namespace APP
