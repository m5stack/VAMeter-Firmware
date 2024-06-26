/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "app.h"
#include <mooncake.h>
#include "assets/assets.h"
#include "hal/hal.h"
#include "apps/apps.h"

using namespace MOONCAKE;
static Mooncake* _mooncake = nullptr;

void APP::Setup(SetupCallback_t callback)
{
    spdlog::info("app setup");

    /* -------------------------------------------------------------------------- */
    /*                            Asset pool injection                            */
    /* -------------------------------------------------------------------------- */
    spdlog::info("asset pool injection");
    if (callback.AssetPoolInjection != nullptr)
        callback.AssetPoolInjection();
    else
        spdlog::warn("empty callback");

    /* -------------------------------------------------------------------------- */
    /*                                HAL injection                               */
    /* -------------------------------------------------------------------------- */
    spdlog::info("hal injection");
    if (callback.HalInjection != nullptr)
        callback.HalInjection();
    else
        spdlog::warn("empty callback");

    // Set locale
    AssetPool::SetLocaleCode(HAL::GetSystemConfig().localeCode);

    /* -------------------------------------------------------------------------- */
    /*                                Mooncake apps                               */
    /* -------------------------------------------------------------------------- */
    _mooncake = new Mooncake;
    _mooncake->init();

    app_run_startup_anim(_mooncake);
    app_install_launcher(_mooncake);
    app_install_apps(_mooncake);
}

void APP::Loop()
{
    _mooncake->update();
    HAL::FeedTheDog();
}

void APP::Destroy()
{
    delete _mooncake;
    HAL::Destroy();
    spdlog::warn("app destroy");
}
