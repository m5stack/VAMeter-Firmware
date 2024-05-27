/**
 * @file assets.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "assets.h"
#include "localization/types.h"
#include <cstdint>
#include <mooncake.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <string>
#ifndef ESP_PLATFORM
#include "fonts/fonts.h"
#include "images/images.h"
#include <iostream>
#include <fstream>
#endif

AssetPool* AssetPool::_asset_pool = nullptr;

AssetPool* AssetPool::Get()
{
    if (_asset_pool == nullptr)
        _asset_pool = new AssetPool;
    return _asset_pool;
}

StaticAsset_t* AssetPool::getStaticAsset()
{
    if (_data.static_asset == nullptr)
    {
        spdlog::error("static asset not exsit");
        return nullptr;
    }
    return _data.static_asset;
}

bool AssetPool::injectStaticAsset(StaticAsset_t* asset)
{
    if (_data.static_asset != nullptr)
    {
        spdlog::error("static asset already exist");
        return false;
    }

    if (asset == nullptr)
    {
        spdlog::error("invalid static asset ptr");
        return false;
    }

    _data.static_asset = asset;

    // Default local text map
    setLocalTextTo(_data.locale_code);
    _create_efont_pool();

    spdlog::info("static asset injected");
    return true;
}

void AssetPool::setLocalTextTo(LocaleCode_t code)
{
    if (code == locale_code_en)
        getStaticAsset()->Text.TextEN.getMap(_data.local_text_pool_map);
    else if (code == locale_code_cn)
        getStaticAsset()->Text.TextCN.getMap(_data.local_text_pool_map);
    else if (code == locale_code_jp)
        getStaticAsset()->Text.TextJP.getMap(_data.local_text_pool_map);
}

void AssetPool::setLocaleCode(LocaleCode_t code)
{
    _data.locale_code = code;
    setLocalTextTo(_data.locale_code);
}

void AssetPool::_create_efont_pool()
{
    _data.efont_pool.eFontCN_16_subset = new lgfx::U8g2font(getStaticAsset()->Font.efontCN_16_subset_data);
    _data.efont_pool.eFontCN_24_subset = new lgfx::U8g2font(getStaticAsset()->Font.efontCN_24_subset_data);
    _data.efont_pool.eFontJA_16_subset = new lgfx::U8g2font(getStaticAsset()->Font.efontJA_16_subset_data);
    _data.efont_pool.eFontJA_24_subset = new lgfx::U8g2font(getStaticAsset()->Font.efontJA_24_subset_data);
}

void AssetPool::loadFont14(LGFX_SpriteFx* lgfxDevice)
{
    // Only en for 14
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibolditalic_14);
}

void AssetPool::loadFont16(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    if (_data.locale_code == locale_code_en)
        lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibold_16);
    else if (_data.locale_code == locale_code_cn)
        lgfxDevice->setFont(_data.efont_pool.eFontCN_16_subset);
    else if (_data.locale_code == locale_code_jp)
        lgfxDevice->setFont(_data.efont_pool.eFontJA_16_subset);
}

void AssetPool::loadFont24(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    // if (_data.locale_code == locale_code_en)
    //     lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibolditalic_24);
    // else if (_data.locale_code == locale_code_cn)
    //     lgfxDevice->setFont(_data.efont_pool.eFontCN_24_subset);
    // else if (_data.locale_code == locale_code_jp)
    //     lgfxDevice->setFont(_data.efont_pool.eFontJA_24_subset);

    if (_data.locale_code == locale_code_en)
        lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibolditalic_24);
    else if (_data.locale_code == locale_code_cn)
        lgfxDevice->setFont(_data.efont_pool.eFontCN_24_subset);
    else if (_data.locale_code == locale_code_jp)
        lgfxDevice->setFont(_data.efont_pool.eFontJA_24_subset);
}

void AssetPool::loadFont72(LGFX_SpriteFx* lgfxDevice)
{
    lgfxDevice->setTextSize(1);
    lgfxDevice->loadFont(getStaticAsset()->Font.montserrat_semibolditalic_72);
}

/* -------------------------------------------------------------------------- */
/*                            Static asset generate                           */
/* -------------------------------------------------------------------------- */
#ifndef ESP_PLATFORM
/**
 * @brief Copy file into target as binary
 *
 * @param filePath
 * @param target
 * @return true
 * @return false
 */
static bool _copy_file(std::string filePath, uint8_t* target)
{
    spdlog::info("try open {}", filePath);

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        spdlog::error("open failed!", filePath);
        return false;
    }
    std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    spdlog::info("file binary size {}", file_size);

    // Copy and go
    if (target != nullptr)
        file.read(reinterpret_cast<char*>(target), file_size);
    file.close();
    return true;
}

static void _copy_fonts(StaticAsset_t* assetPool)
{
    _copy_file("../../app/assets/fonts/Montserrat-SemiBold-16.vlw", assetPool->Font.montserrat_semibold_16);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBold-24.vlw", assetPool->Font.montserrat_semibold_24);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBold-36.vlw", assetPool->Font.montserrat_semibold_36);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-14.vlw", assetPool->Font.montserrat_semibolditalic_14);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-24.vlw", assetPool->Font.montserrat_semibolditalic_24);
    _copy_file("../../app/assets/fonts/Montserrat-SemiBoldItalic-72.vlw", assetPool->Font.montserrat_semibolditalic_72);

    std::memcpy(assetPool->Font.efontCN_16_subset_data, efontCN_16_subset_data, efontCN_16_subset_data_size);
    std::memcpy(assetPool->Font.efontCN_24_subset_data, efontCN_24_subset_data, efontCN_24_subset_data_size);

    std::memcpy(assetPool->Font.efontJA_16_subset_data, efontJA_16_subset_data, efontJA_16_subset_data_size);
    std::memcpy(assetPool->Font.efontJA_24_subset_data, efontJA_24_subset_data, efontJA_24_subset_data_size);
}

static void _copy_images(StaticAsset_t* assetPool)
{
    std::memcpy(assetPool->Image.AppFile.app_icon, image_data_icon_app_files, image_data_icon_app_files_size);

    std::memcpy(assetPool->Image.AppLauncher.next_icon, image_data_icon_next, image_data_icon_next_size);
    std::memcpy(assetPool->Image.AppLauncher.vameter_logo, image_data_vameter_logo, image_data_vameter_logo_size);
    std::memcpy(
        assetPool->Image.AppLauncher.vameter_logo_light, image_data_vameter_logo_light, image_data_vameter_logo_light_size);
    std::memcpy(assetPool->Image.AppLauncher.guide_map, image_data_guide_map, image_data_guide_map_size);

    std::memcpy(
        assetPool->Image.AppPowerMonitor.app_icon, image_data_icon_app_power_monitor, image_data_icon_app_power_monitor_size);
    std::memcpy(assetPool->Image.AppPowerMonitor.page_bus_power_panel,
                image_data_panel_app_bus_power,
                image_data_panel_app_bus_power_size);
    std::memcpy(assetPool->Image.AppPowerMonitor.page_bus_volt_panel,
                image_data_panel_app_bus_volt,
                image_data_panel_app_bus_volt_size);
    std::memcpy(assetPool->Image.AppPowerMonitor.page_shunt_current_panel,
                image_data_panel_app_shunt_current,
                image_data_panel_app_shunt_current_size);
    std::memcpy(assetPool->Image.AppPowerMonitor.page_shunt_volt_panel,
                image_data_panel_app_shunt_volt,
                image_data_panel_app_shunt_volt_size);
    std::memcpy(assetPool->Image.AppPowerMonitor.spinner_icon, image_data_spinner, image_data_spinner_size);

    std::memcpy(assetPool->Image.AppWaveform.app_icon, image_data_icon_app_waveform, image_data_icon_app_waveform_size);
    std::memcpy(
        assetPool->Image.AppWaveform.current_panel, image_data_panel_app_waveform_a, image_data_panel_app_waveform_a_size);
    std::memcpy(
        assetPool->Image.AppWaveform.voltage_panel, image_data_panel_app_waveform_v, image_data_panel_app_waveform_v_size);

    std::memcpy(assetPool->Image.AppRemote.app_icon, image_data_icon_app_remote, image_data_icon_app_remote_size);

    std::memcpy(assetPool->Image.AppSettings.app_icon, image_data_icon_app_settings, image_data_icon_app_settings_size);
    std::memcpy(
        assetPool->Image.AppSettings.avatar_forairaaaaa, image_data_avatar_forairaaaaa, image_data_avatar_forairaaaaa_size);
    std::memcpy(assetPool->Image.AppSettings.avatar_gtxyj, image_data_avatar_gtxyj, image_data_avatar_gtxyj_size);
}

static void _copy_web_pages(StaticAsset_t* assetPool)
{
    _copy_file("../../app/assets/web/syscfg.html", assetPool->WebPage.syscfg);
    _copy_file("../../app/assets/web/favicon.ico", assetPool->WebPage.favicon);
}

StaticAsset_t* AssetPool::CreateStaticAsset()
{
    auto asset_pool = new StaticAsset_t;

    _copy_fonts(asset_pool);
    _copy_images(asset_pool);
    _copy_web_pages(asset_pool);

    /* -------------------------------------------------------------------------- */
    /*                                Output to bin                               */
    /* -------------------------------------------------------------------------- */
    std::string bin_path = "AssetPool-VAMeter.bin";

    std::ofstream outFile(bin_path, std::ios::binary);
    if (!outFile)
        spdlog::error("open {} failed", bin_path);

    outFile.write(reinterpret_cast<const char*>(asset_pool), sizeof(StaticAsset_t));
    outFile.close();
    spdlog::info("output asset pool to: {}", bin_path);

    return asset_pool;
}

StaticAsset_t* AssetPool::GetStaticAssetFromBin()
{
    auto asset_pool = new StaticAsset_t;

    // Read from bin
    std::string bin_path = "AssetPool-VAMeter.bin";

    std::ifstream inFile(bin_path, std::ios::binary);
    if (!inFile)
        spdlog::error("open {} failed", bin_path);

    inFile.read(reinterpret_cast<char*>(asset_pool), sizeof(StaticAsset_t));
    inFile.close();

    // // Test
    // for (int i = 0; i < 10; i++)
    // {
    //     spdlog::info(
    //         "0x{:X} 0x{:X}", asset_pool->Font.montserrat_semibold_14[i], asset_pool->Font.montserrat_semibolditalic_72[i]);
    // }

    spdlog::info("load asset pool from: {}", bin_path);
    return asset_pool;
}
#endif
