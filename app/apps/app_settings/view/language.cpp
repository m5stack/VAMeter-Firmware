/**
 * @file language.cpp
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
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /language
void AppSettings::_on_page_language()
{
    spdlog::info("on page language");
    auto history_locale_code = HAL::GetSystemConfig().localeCode;

    int selected_index = static_cast<int>(HAL::GetSystemConfig().localeCode);

    while (1)
    {
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppSettings_Option_English);
        options.push_back(AssetPool::GetText().AppSettings_Option_Chinese);
        options.push_back(AssetPool::GetText().AppSettings_Option_Japanese);
        options.push_back(AssetPool::GetText().AppSettings_Option_Back);

        // Custom render callback
        auto render_callback = [](int optionIndex, int x, int y) {
            if (optionIndex == 0)
            {
                AssetPool::LoadFont24(HAL::GetCanvas());
                HAL::GetCanvas()->drawString(AssetPool::GetTextPool().TextEN.AppSettings_Option_English, x, y);
            }
            else if (optionIndex == 1)
            {
                HAL::GetCanvas()->setFont(AssetPool::GetEFontPool().eFontCN_24_subset);
                HAL::GetCanvas()->drawString(AssetPool::GetTextPool().TextCN.AppSettings_Option_Chinese, x, y);
            }
            else if (optionIndex == 2)
            {
                HAL::GetCanvas()->setFont(AssetPool::GetEFontPool().eFontJA_24_subset);
                HAL::GetCanvas()->drawString(AssetPool::GetTextPool().TextJP.AppSettings_Option_Japanese, x, y);
            }
            else if (optionIndex == 3)
            {
                AssetPool::LoadFont24(HAL::GetCanvas());
                HAL::GetCanvas()->drawString(AssetPool::GetText().AppSettings_Option_Back, x, y);
            }
        };

        selected_index = SelectMenuPage::CreateAndWaitResult(AssetPool::GetText().AppSettings_Option_Language,
                                                             options,
                                                             selected_index,
                                                             &_data.select_page_theme,
                                                             render_callback);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        // Apply change
        AssetPool::SetLocaleCode(static_cast<LocaleCode_t>(selected_index));
        HAL::GetSystemConfig().localeCode = static_cast<LocaleCode_t>(selected_index);

        break;
    }

    // spdlog::info("{} {}", (int)history_locale_code, (int)HAL::GetSystemConfig().localeCode);
    if (history_locale_code != HAL::GetSystemConfig().localeCode)
    {
        HAL::SaveSystemConfig();
    }
}
