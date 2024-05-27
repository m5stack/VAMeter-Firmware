/**
 * @file hal_ota.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-28
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_event.h>
#include <esp_ota_ops.h>
#include <esp_http_client.h>
#include <esp_https_ota.h>
#include <esp_wifi.h>
extern "C"
{
#include "../utils/wifi/wifi.h"
#include "../utils/simple_ota/simple_ota.h"
}
#include <assets/assets.h>

/* -------------------------------------------------------------------------- */
/*                                Get OTA info                                */
/* -------------------------------------------------------------------------- */
static const char* _ota_server_host = "https://ota.m5stack.com";
static const char* _ota_api_check_latest = "/ota/api/v2/firmware/getFirmwareVersion";

OTA_UPGRADE::OtaInfo_t HAL_VAMeter::getLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender)
{
    OTA_UPGRADE::OtaInfo_t info;

    HTTPClient http;
    {
        std::string url = _ota_server_host;
        url += _ota_api_check_latest;
        url += "?mac=";
        url += _get_mac();
        url += "&sku=K136&version=";
        url += version();

        spdlog::info("get ota data on: {}", url);
        http.begin(url.c_str());
    }

    // Fetch info
    auto response_code = http.GET();
    if (response_code > 0)
    {
        JsonDocument doc;
        DeserializationError error;

        // Parse payload
        {
            String response = http.getString();
            http.end();

            spdlog::info("get response:\n\n {}\n", response.c_str());
            error = deserializeJson(doc, response.c_str());
        }

        // If parse failed
        if (error != DeserializationError::Ok)
        {
            info.getInfoFailed = true;
            info.firmwareUrl = "Json parsing failed";

            spdlog::error("{}: {}", info.firmwareUrl, error.c_str());
            return info;
        }

        // Copy info
        else
        {
            // If shit happened
            if (doc["code"].as<int>() != 200)
            {
                info.getInfoFailed = true;
                info.firmwareUrl = "Bad response: ";
                info.firmwareUrl += std::to_string(doc["code"].as<int>());

                spdlog::error("{}", info.firmwareUrl);
                return info;
            }

            // Copy
            info.latestVersion = doc["data"]["version"].as<std::string>();
            info.upgradeAvailable = doc["data"]["needUpgrade"];
            info.firmwareUrl = doc["data"]["url"].as<std::string>();
            info.description = doc["data"]["description"].as<std::string>();
            info.needUsbFlashing = doc["data"]["needUsbFlashing"];

            spdlog::info("get info:\n - latestVersion: {}\n - upgradeAvailable: {}\n - firmwareUrl: {}\n - description:\n {}\n "
                         "- needUsbFlashing: {}",
                         info.latestVersion,
                         info.upgradeAvailable,
                         info.firmwareUrl,
                         info.description,
                         info.needUsbFlashing);
            return info;
        }
    }
    // If get failed
    else
    {
        http.end();

        info.getInfoFailed = true;
        info.firmwareUrl = "Bad HTTP request";

        spdlog::error("{}: {}", info.firmwareUrl, response_code);
        return info;
    }

    return info;
}

/* -------------------------------------------------------------------------- */
/*                                  OTA write                                 */
/* -------------------------------------------------------------------------- */
static OnLogPageRenderCallback_t _on_log_page_render_temp = nullptr;

bool HAL_VAMeter::upgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl)
{
    if (firmwareUrl.empty())
    {
        popWarning("Empty URL");
        return false;
    }

    if (!connectWifi(onLogPageRender, false))
        return false;

    std::string string_buffer = spdlog::fmt_lib::format(" > ota url:\n > {}\n > start ota..\n", firmwareUrl);
    onLogPageRender(string_buffer, true, false);

    // Set logging callback
    _on_log_page_render_temp = onLogPageRender;
    auto ota_on_log_callback = [](char* log) { _on_log_page_render_temp(log, true, false); };

    // Start ota
    if (simple_ota_start_via_http(firmwareUrl.c_str(), ota_on_log_callback))
    {
        popSuccess(AssetPool::GetText().Misc_Text_UpgradeDone);

        // Reboot
        spdlog::info("reboot..");
        delay(100);
        esp_restart();
        while (1)
            delay(1000);
    }

    popWarning("OTA Failed");
    return false;
}
