/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
// Ref: https://github.com/nayuki/QR-Code-generator/blob/master/cpp/QrCodeGeneratorDemo.cpp
#include "qrcode.h"
#include "qrcodegen/qrcodegen.hpp"
#include "../../../hal/hal.h"
#include <mooncake.h>
#include "spdlog/spdlog.h"
#include <smooth_ui_toolkit.h>
#include <cassert>
#include <cmath>

using qrcodegen::QrCode;
using namespace SmoothUIToolKit;

void QRCODE::RenderQRCode(int x, int y, const char* text, int size, uint32_t colorOn, uint32_t colorOff)
{
    // Get module
    const QrCode qr = QrCode::encodeText(text, QrCode::Ecc::LOW);
    spdlog::info("qr size {}", qr.getSize());

    // Get scale
    assert(size > qr.getSize());
    int scale = std::round((float)size / qr.getSize());
    spdlog::info("scale {}", scale);

    // Render
    Vector2D_t qr_image_cursor(x, y);
    for (int qr_y = 0; qr_y < qr.getSize(); qr_y++)
    {
        for (int qr_x = 0; qr_x < qr.getSize(); qr_x++)
        {
            // spdlog::info("{} {} {}", qr_image_cursor.x, qr_image_cursor.y, qr.getModule(qr_x, qr_y));

            HAL::GetCanvas()->fillRect(
                qr_image_cursor.x, qr_image_cursor.y, scale, scale, qr.getModule(qr_x, qr_y) ? colorOn : colorOff);

            qr_image_cursor.x += scale;
        }
        qr_image_cursor.x = x;
        qr_image_cursor.y += scale;
    }
}

void QRCODE::GetQrcodeBitmap(std::vector<std::vector<bool>>& bitmap, const char* text)
{
    // Get module
    const QrCode qr = QrCode::encodeText(text, QrCode::Ecc::LOW);
    spdlog::info("qr size {}", qr.getSize());

    bitmap.clear();
    bitmap.resize(qr.getSize());
    for (int qr_y = 0; qr_y < qr.getSize(); qr_y++)
    {
        bitmap[qr_y].resize(qr.getSize());
        for (int qr_x = 0; qr_x < qr.getSize(); qr_x++)
        {
            bitmap[qr_y][qr_x] = qr.getModule(qr_x, qr_y);
        }
    }
    spdlog::info("bitmap size {}", bitmap.size());
}

void QRCODE::RenderQRCodeBitmap(
    std::vector<std::vector<bool>>& bitmap, int x, int y, int size, uint32_t colorOn, uint32_t colorOff)
{
    // Get scale
    assert(size > bitmap.size());
    int scale = std::round((float)size / bitmap.size());
    // spdlog::info("scale {}", scale);

    Vector2D_t qr_image_cursor(x, y);
    for (const auto& line : bitmap)
    {
        for (const auto& bit : line)
        {
            // spdlog::info("{} {} {}", qr_image_cursor.x, qr_image_cursor.y, bit);
            HAL::GetCanvas()->fillRect(qr_image_cursor.x, qr_image_cursor.y, scale, scale, bit ? colorOn : colorOff);
            qr_image_cursor.x += scale;
        }
        qr_image_cursor.x = x;
        qr_image_cursor.y += scale;
    }
}
