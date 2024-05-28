/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
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

// Page: /record_files
void AppFiles::_on_page_record_files()
{
    spdlog::info("on page record files");

HELL:
    // Get file list
    auto file_list = HAL::GetVaRecordNameList();
    if (file_list.size() == 0)
    {
        HAL::PopWarning(AssetPool::GetText().Misc_Text_NoRecordFiles);
        return;
    }
    file_list.push_back(AssetPool::GetText().AppSettings_Option_Back);

    int selected_index = 0;
    while (1)
    {
        selected_index =
            SelectMenuPage::CreateAndWaitResult(AssetPool::GetText().AppFiles_Option_RecordFiles, file_list, selected_index);

        // Check quit
        if (selected_index == -1)
            break;
        else if (selected_index == file_list.size() - 1)
            break;

        _on_record_file_open(file_list[selected_index]);

        if (_data.is_just_delete_file)
            goto HELL;
    }
}

// Page: /local_files/rec_xx_open
void AppFiles::_on_record_file_open(const std::string& fileName)
{
    spdlog::info("on page record file open");

    int selected_index = 0;
    while (1)
    {
        std::vector<std::string> option_list;
        option_list.push_back(AssetPool::GetText().AppFiles_Option_Open);
        option_list.push_back(AssetPool::GetText().AppFiles_Option_Upload);
        option_list.push_back(AssetPool::GetText().AppFiles_Option_Delete);
        option_list.push_back(AssetPool::GetText().AppSettings_Option_Back);

        selected_index = SelectMenuPage::CreateAndWaitResult(fileName.c_str(), option_list, selected_index);

        // Quit
        if (selected_index == -1)
            break;
        else if (selected_index == option_list.size() - 1)
            break;

        // Preview
        else if (selected_index == 0)
        {
            auto record = HAL::GetVaRecord(fileName);
            VaRecordViewer::CreateAndWait(&record);
        }

        // Upload
        else if (selected_index == 1)
        {
            if (HAL::UploadVaRecordViaEzData(fileName, OnLogPageRender))
                _on_page_upload_success(fileName);
        }

        // Delete
        else if (selected_index == 2)
        {
            HAL::DeleteVaRecord(fileName);
            _data.is_just_delete_file = true;
            break;
        }
    }
}
