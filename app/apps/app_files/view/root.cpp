/**
 * @file root.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../app_files.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../../assets/assets.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;

// Page: /
void AppFiles::_on_page_root()
{
    spdlog::info("on page root");

    int selected_index = 0;
    while (1)
    {
        // std::vector<std::string> options = {"- Record files", "- MSC mode", "- Quit"};
        std::vector<std::string> options;
        options.push_back(AssetPool::GetText().AppFiles_Option_RecordFiles);
        options.push_back(AssetPool::GetText().AppFiles_Option_MscMode);
        options.push_back(AssetPool::GetText().AppSettings_Option_Quit);

        selected_index = SelectMenuPage::CreateAndWaitResult(AssetPool::GetText().AppName_Files, options, selected_index);

        if (selected_index == -1)
            break;
        else if (selected_index == options.size() - 1)
            break;

        else if (selected_index == 0)
            _on_page_record_files();

        else if (selected_index == 1)
            _on_page_msc_mode();
    }
}
