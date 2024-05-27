/**
 * @file app_startup_anim.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app_startup_anim.h"
#include "../../hal/hal.h"
#include "../../assets/assets.h"
#include "../utils/system/system.h"
#include <cstdint>
#include <smooth_ui_toolkit.h>
#include <mooncake.h>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SmoothUIToolKit;

static void _render_startup_image()
{
    if (!HAL::RenderCustomStartupImage())
    {
        spdlog::info("no custom startup image");
        if (HAL::GetSystemConfig().startupImage == DEFAULT_STARTUP_IMAGE_LIGHT_TAG)
        {
            HAL::GetCanvas()->fillScreen((uint32_t)0xF5F5F5);
            HAL::GetCanvas()->pushImage(53, 51, 135, 103, AssetPool::GetStaticAsset()->Image.AppLauncher.vameter_logo_light);
        }
        else
        {
            HAL::GetCanvas()->fillScreen(TFT_BLACK);
            HAL::GetCanvas()->pushImage(53, 51, 135, 103, AssetPool::GetStaticAsset()->Image.AppLauncher.vameter_logo);
        }
    }
    HAL::CanvasUpdate();
}

static void _smooth_backlight_on()
{
    // Brightness easing
    Transition2D transition;
    transition.jumpTo(0, 0);
    transition.moveTo(0, HAL::GetSystemConfig().brightness);
    transition.setDuration(600);
    while (!transition.isFinish())
    {
        transition.update(HAL::Millis());
        HAL::GetDisplay()->setBrightness(transition.getValue().y);
        HAL::FeedTheDog();
    }

    // More time to enjoy your startup image
    HAL::Delay(700);
}

static void _pop_up_launcher_background_mask()
{
    Transition2D transition;

    // Guess which type-c port you pluged in
    HAL::UpdatePowerMonitor();
    if (HAL::GetPowerMonitorData().busVoltage >= 3.0)
        transition.jumpTo(-200, 40);
    else
        transition.jumpTo(120, -200);

    // A circle as mask
    transition.setDuration(300);
    transition.moveTo(120, 120);
    while (!transition.isFinish())
    {
        transition.update(HAL::Millis());
        HAL::GetCanvas()->fillSmoothCircle(
            transition.getValue().x, transition.getValue().y, 200, AssetPool::GetColor().AppLauncher.background);
        HAL::CanvasUpdate();
        HAL::FeedTheDog();
    }
}

void AppStartupAnim::PopUpGuideMap(bool force)
{
    if (!force && HAL::GetSystemConfig().bootCount > 6)
        return;

    Transition2D transition;
    transition.setTransitionPath(EasingPath::easeOutBack);
    transition.setDuration(600);
    transition.jumpTo(0, 200);
    transition.moveTo(0, 0);

    while (!transition.isFinish())
    {
        transition.update(HAL::Millis());
        HAL::GetCanvas()->pushImage(
            transition.getValue().x, transition.getValue().y, 240, 240, AssetPool::GetImage().AppLauncher.guide_map);
        HAL::CanvasUpdate();
        HAL::FeedTheDog();
    }

    while (1)
    {
        HAL::Delay(50);
        HAL::FeedTheDog();

        Encoder::Update();
        Button::Update();
        if (Button::Side()->wasHold())
            break;
        if (Button::Encoder()->wasClicked())
            break;
    }
}

// Like setup()...
void AppStartupAnim::onResume()
{
    spdlog::info("{} onResume", getAppName());

    if (HAL::GetSystemConfig().startupImage != PASS_STARTUP_IMAGE_TAG)
    {
        _render_startup_image();
        _smooth_backlight_on();
    }
    else
    {
        // Wait pm data ready
        HAL::Delay(200);
        spdlog::info("pass startup image");
        HAL::GetDisplay()->setBrightness(HAL::GetSystemConfig().brightness);
    }

    PopUpGuideMap();
    _pop_up_launcher_background_mask();

    destroyApp();
}

void AppStartupAnim::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());

    // Release resources here..
    // delete 114514;
}
