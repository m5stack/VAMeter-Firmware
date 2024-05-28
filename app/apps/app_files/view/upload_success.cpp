/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../app_files.h"
#include "../../../hal/hal.h"
#include "../../utils/system/system.h"
#include "../../utils/qrcode/qrcode.h"
#include "../../../assets/assets.h"
#include "hal/utils/lgfx_fx/lgfx_fx.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/fmt/bundled/core.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>

using namespace SYSTEM::INPUTS;
using namespace SYSTEM::UI;
using namespace SmoothUIToolKit;
using namespace MOONCAKE::APPS;
using namespace QRCODE;

void AppFiles::_on_page_upload_success(const std::string& recordName)
{
    spdlog::info("on page upload success");
    // HAL::PopSuccess(AssetPool::GetText().Misc_Text_UploadDone);

    // Pop url qrcode
    auto url = HAL::GetVaRecordEzDataUrl();
    std::vector<std::vector<bool>> qrcode_bitmap;
    GetQrcodeBitmap(qrcode_bitmap, url.c_str());

    CreateNoticePage(
        "Success",
        [&](Transition2D& transition) {
            // Qrcode
            RenderQRCodeBitmap(qrcode_bitmap,
                               120 - 120 / 2,
                               50 + transition.getValue().y,
                               120,
                               AssetPool::GetColor().AppSettings.optionText,
                               AssetPool::GetColor().AppSettings.background);

            HAL::GetCanvas()->loadFont(AssetPool::GetStaticAsset()->Font.montserrat_semibolditalic_14);
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->setTextColor(AssetPool::GetColor().AppSettings.optionTextLight);
            HAL::GetCanvas()->drawString("-----   Check Online   -----", 120, 185 + transition.getValue().y);

            // Lable
            AssetPool::LoadFont24(HAL::GetCanvas());
            HAL::GetCanvas()->setTextDatum(middle_center);
            HAL::GetCanvas()->setTextColor(AssetPool::GetStaticAsset()->Color.AppSettings.optionText);
            std::string string_buffer = fmt::format("{}", AssetPool::GetText().Misc_Text_UploadDone);
            HAL::GetCanvas()->drawString(string_buffer.c_str(), 120, 215 + transition.getValue().y);
        },
        nullptr);
}
