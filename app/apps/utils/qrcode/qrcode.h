/**
 * @file qrcode.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-05-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <cstdint>
#include <vector>

namespace QRCODE
{
    // Slow
    void RenderQRCode(int x, int y, const char* text, int size = 100, uint32_t colorOn = 0x0, uint32_t colorOff = 0xFFFFFF);

    // Separate
    void GetQrcodeBitmap(std::vector<std::vector<bool>>& bitmap, const char* text);
    void RenderQRCodeBitmap(std::vector<std::vector<bool>>& bitmap,
                      int x,
                      int y,
                      int size = 100,
                      uint32_t colorOn = 0x0,
                      uint32_t colorOff = 0xFFFFFF);
} // namespace QRCODE
