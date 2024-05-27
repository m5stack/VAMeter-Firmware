/**
 * @file types.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                   Images                                   */
/* -------------------------------------------------------------------------- */
struct ImagePool_t
{
    struct AppFiles_t
    {
        uint16_t app_icon[10000];
    };
    AppFiles_t AppFile;

    struct AppLauncher_t
    {
        uint16_t next_icon[400];
        uint16_t vameter_logo[13905];
        uint16_t vameter_logo_light[13905];
        uint16_t guide_map[57600];
    };
    AppLauncher_t AppLauncher;

    struct AppPowerMonitor_t
    {
        uint16_t app_icon[10000];
        uint16_t page_bus_power_panel[12350];
        uint16_t page_bus_volt_panel[12350];
        uint16_t page_shunt_current_panel[12350];
        uint16_t page_shunt_volt_panel[12350];
        uint16_t spinner_icon[1369];
    };
    AppPowerMonitor_t AppPowerMonitor;

    struct AppWaveform_t
    {
        uint16_t app_icon[10000];
        uint16_t current_panel[6480];
        uint16_t voltage_panel[6480];
    };
    AppWaveform_t AppWaveform;

    struct AppRemote_t
    {
        uint16_t app_icon[10000];
    };
    AppRemote_t AppRemote;

    struct AppSettings_t
    {
        uint16_t app_icon[10000];
        uint16_t avatar_forairaaaaa[2304];
        uint16_t avatar_gtxyj[2304];
    };
    AppSettings_t AppSettings;
};
