/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "app_launcher.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "core/transition3d/transition3d.h"
#include "spdlog/spdlog.h"

using namespace MOONCAKE::APPS;
using namespace SmoothUIToolKit;

void AppLauncher::onCreate()
{
    spdlog::info("{} onCreate", getAppName());
    startApp();
    setAllowBgRunning(true);
}

static bool _is_just_boot_up = true;

void AppLauncher::onResume()
{
    spdlog::info("{} onResume", getAppName());

    // If just boot up, open history app directly
    if (_is_just_boot_up)
    {
        _is_just_boot_up = false;

        // Create app
        if (HAL::NvsGet(NVS_KEY_APP_HISTORY) == 5)
        {
            mcAppGetFramework()->createAndStartApp(mcAppGetFramework()->getInstalledAppList()[2]);
            VIEW::LauncherView::SetLastSelectedOptionIndex(1);
        }
        else
        {
            mcAppGetFramework()->createAndStartApp(mcAppGetFramework()->getInstalledAppList()[1]);
            VIEW::LauncherView::SetLastSelectedOptionIndex(0);
        }

        // Stack launcher into background
        closeApp();

        return;
    }

    _create_launcher_view();
}

void AppLauncher::onRunning() { _update_launcher_view(); }

void AppLauncher::onRunningBG()
{
    // If only launcher left
    if (mcAppGetFramework()->getAppManager()->getCreatedAppNum() <= 1)
    {
        // Back to the game
        startApp();
    }
}

void AppLauncher::onPause() { _destroy_launcher_view(); }

void AppLauncher::onDestroy() { spdlog::info("{} onDestroy", getAppName()); }

/* -------------------------------------------------------------------------- */
/*                                    View                                    */
/* -------------------------------------------------------------------------- */
void AppLauncher::_create_launcher_view()
{
    // Create menu
    _data.launcher_view = new VIEW::LauncherView;

    // Add app option
    for (const auto& app_packer : mcAppGetFramework()->getInstalledAppList())
    {
        // Pass launcher
        if (app_packer == getAppPacker())
            continue;

        VIEW::LauncherView::AppOptionProps_t new_app_option;

        if (app_packer->getCustomData() != nullptr)
            new_app_option.themeColor = *(uint32_t*)app_packer->getCustomData();
        new_app_option.icon = app_packer->getAppIcon();
        new_app_option.name = app_packer->getAppName();

        _data.launcher_view->addAppOption(new_app_option);
    }

    // App opened callback
    _data.launcher_view->setAppOpenCallback([&](int selectedIndex) {
        spdlog::info("open app by packer index: {}", selectedIndex + 1);

        // Create and start app, +1 to pass the launcher itself (app 0)
        mcAppGetFramework()->createAndStartApp(mcAppGetFramework()->getInstalledAppList()[selectedIndex + 1]);

        // Stack launcher into background
        closeApp();
    });

    _data.launcher_view->init();
}

void AppLauncher::_update_launcher_view() { _data.launcher_view->update(HAL::Millis()); }

void AppLauncher::_destroy_launcher_view()
{
    if (_data.launcher_view != nullptr)
    {
        delete _data.launcher_view;
        _data.launcher_view = nullptr;
    }
}
