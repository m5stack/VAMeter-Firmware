/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
/**
 * @file encoder.cpp
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
#include "../../utils/easter_egg/easter_egg.h"
#include "lgfx/v1/misc/enum.hpp"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /factory reset
void AppSettings::_on_factory_reset()
{
    spdlog::info("on factory reset");

    if (!CreateNoticePage("Factory Reset",
                          [](Transition2D& transition)
                          {
                              HAL::GetCanvas()->setTextColor(TFT_RED);
                              HAL::GetCanvas()->printf(
                                  "\n [WARNING]\n - All files and config\n - On the device\n - Will be deleted");

                              HAL::GetCanvas()->drawCenterString(
                                  AssetPool::GetText().Misc_Text_ClickToContinue, 120, 196 + transition.getValue().y);
                              HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
                          }))
        return;

    if (!CreateNoticePage("Factory Reset",
                          [](Transition2D& transition)
                          {
                              HAL::GetCanvas()->setTextColor(TFT_RED);
                              HAL::GetCanvas()->printf("\n Sure?");

                              HAL::GetCanvas()->drawCenterString(
                                  AssetPool::GetText().Misc_Text_ClickToContinue, 120, 196 + transition.getValue().y);
                              HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
                          }))
        return;

    if (!CreateNoticePage("Factory Reset",
                          [](Transition2D& transition)
                          {
                              HAL::GetCanvas()->setTextColor(TFT_RED);
                              HAL::GetCanvas()->printf("\n Really Sure?");

                              HAL::GetCanvas()->drawCenterString(
                                  AssetPool::GetText().Misc_Text_ClickToContinue, 120, 196 + transition.getValue().y);
                              HAL::GetCanvas()->drawCenterString("V", 120, 216 + transition.getValue().y);
                          }))
        return;

    HAL::FactoryReset(OnLogPageRender);
}
