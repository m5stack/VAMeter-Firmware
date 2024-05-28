/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                   Colors                                   */
/* -------------------------------------------------------------------------- */
struct ColorPool_t
{
    struct AppFiles_t
    {
        uint32_t background = 0xFFFFFF;
        uint32_t optionText = 0x300E00;
        uint32_t selector = 0xF1883D;
    };
    AppFiles_t AppFile;

    struct AppLauncher_t
    {
        uint32_t background = 0xCBDEEE;
        uint32_t selector = 0x536484;
        uint32_t iconBackgound = 0xFFFFFF;
        uint32_t panel = 0x334464;
    };
    AppLauncher_t AppLauncher;

    struct AppPowerMonitor_t
    {
        // uint32_t primary = 0x0058FF;
        uint32_t primary = 0x0058FF;

        // uint32_t pageBusVoltage = 0x0058FF;
        // uint32_t pageShuntCurrent = 0xFF3333;
        // uint32_t pageBusPower = 0x009653;
        // uint32_t pageShuntVoltage = 0x8D00FF;
        uint32_t pageBusVoltage = 0x0058FF;
        uint32_t pageShuntCurrent = 0xFF0066;
        uint32_t pageBusPower = 0x009653;
        uint32_t pageShuntVoltage = 0x8D00FF;
        uint32_t pageTime = 0x191919;

        uint32_t pageSimpleDetailBackground = 0xFFFFFF;
        uint32_t pageSimpleDetailText = 0xFFFFFF;

        uint32_t pageMoreDetailBackground = 0xFFFFFF;
        uint32_t pageMoreDetailPanel = 0xCBE8FF;
        uint32_t pageMoreDetailLabelBasic = 0x718092;
        uint32_t pageMoreDetailLabelValue = 0x21567C;
        uint32_t pageMoreDetailAvgHightLight = 0xE84141;
        uint32_t pageMoreDetailCurrentHightLight = 0x2D69EB;
        uint32_t pageMoreDetailCapacityHightLight = 0x0F7E49;
    };
    AppPowerMonitor_t AppPowerMonitor;

    struct AppWaveform_t
    {
        uint32_t primary = 0xFF6161;

        uint32_t colorGuideLine = 0xD4D4D4;
        uint32_t colorLineV = 0x0000FF;
        uint32_t colorLineA = 0xFF0000;

        uint32_t colorConfigPanel = 0x035E36;
        uint32_t colorThresholdLine = 0x361981;
        uint32_t colorTriggerModeIcon = 0x361981;
        uint32_t colorStateLabelWaiting = 0xF1883D;
        uint32_t colorStateLabelRecording = 0xFF0000;
    };
    AppWaveform_t AppWaveform;

    struct AppRemote_t
    {
        uint32_t background = 0xDCD6FF;
        uint32_t optionText = 0x50478A;
        uint32_t selector = 0xAA9CFF;
    };
    AppRemote_t AppRemote;

    struct AppSettings_t
    {
        uint32_t background = 0xFFFFFF;
        uint32_t optionText = 0x090044;
        uint32_t selector = 0x9B8CFF;
        uint32_t optionTextLight = 0x5A567A;
    };
    AppSettings_t AppSettings;

    struct Misc_t
    {
        uint32_t bgPopFatalError = 0x0078d7;
        uint32_t bgPopWarning = 0xFE8B00;
        uint32_t bgPopSuccess = 0x009653;
    };
    Misc_t Misc;
};
