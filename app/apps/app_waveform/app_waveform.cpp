/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "app_waveform.h"
#include "../../hal/hal.h"
#include "../utils/system/system.h"
#include "../../assets/assets.h"
#include "../app_files/app_files.h"
#include "apps/utils/system/inputs/button/button.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>

using namespace MOONCAKE::APPS;
using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;

const char* AppWaveform_Packer::getAppName() { return AssetPool::GetText().AppName_Waveform; }

// Theme color
void* AppWaveform_Packer::getCustomData() { return (void*)(&AssetPool::GetColor().AppWaveform.primary); }

// Icon
void* AppWaveform_Packer::getAppIcon() { return (void*)AssetPool::GetStaticAsset()->Image.AppWaveform.app_icon; }

// Like setup()...
void AppWaveform::onResume()
{
    spdlog::info("{} onResume", getAppName());
    _data.view = new VIEWS::WaveFormRecorder(AssetPool::GetColor().AppWaveform.primary);
    _data.view->init();
}

// Like loop()...
void AppWaveform::onRunning()
{
    _data.view->update();

    // Check kill signal
    if (_data.view->want2quit())
        destroyApp();

    // Check finish
    if (_data.view->hasFinishedRecording())
        _handle_recording_finished();
}

void AppWaveform::onDestroy()
{
    spdlog::info("{} onDestroy", getAppName());
    delete _data.view;
}

void AppWaveform::_handle_recording_finished()
{
    // Destroy view
    delete _data.view;

    // What to do with
    // std::vector<std::string> option_list = {" - Open", " - Upload (EzData)", " - Delete", " - Continue"};
    std::vector<std::string> option_list;
    option_list.push_back(AssetPool::GetText().AppFiles_Option_Open);
    option_list.push_back(AssetPool::GetText().AppFiles_Option_Upload);
    option_list.push_back(AssetPool::GetText().AppFiles_Option_Delete);
    option_list.push_back(AssetPool::GetText().AppSettings_Option_Back);

    SelectMenuPage::Theme_t theme;
    theme.background = AssetPool::GetColor().AppFile.background;
    theme.optionText = AssetPool::GetColor().AppFile.optionText;
    theme.selector = AssetPool::GetColor().AppFile.selector;

    while (1)
    {
        auto selected_index = SelectMenuPage::CreateAndWaitResult(HAL::GetLatestVaRecordName().c_str(), option_list, 0, &theme);

        // Quit
        if (selected_index == -1)
            break;
        else if (selected_index == option_list.size() - 1)
            break;

        // Preview
        else if (selected_index == 0)
        {
            auto record = HAL::GetLatestVaRecord();
            VaRecordViewer::CreateAndWait(&record);
        }

        // Upload
        else if (selected_index == 1)
        {
            if (HAL::UploadVaRecordViaEzData(HAL::GetLatestVaRecordName(), OnLogPageRender))
                AppFiles::_on_page_upload_success(HAL::GetLatestVaRecordName());
        }

        // Delete
        else if (selected_index == 2)
        {
            HAL::DeleteVaRecord(HAL::GetLatestVaRecordName());
            break;
        }
    }

    // Recreate view
    _data.view = new VIEWS::WaveFormRecorder(AssetPool::GetColor().AppWaveform.primary);
    _data.view->init();
}
