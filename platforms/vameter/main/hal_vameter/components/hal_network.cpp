/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
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
}
#include <assets/assets.h>

/* --------------------------------- IDF way -------------------------------- */
// bool HAL_VAMeter::_try_connect_wifi_with_logging_page(OnLogPageRenderCallback_t onLogPageRender)
// {

//     // Check config
//     if (!checkWifiConfig())
//         return false;

//     std::string string_buffer;

//     string_buffer = spdlog::fmt_lib::format(" > try connecting wifi..\n > ssid: {}\n", _config.wifiSsid);
//     onLogPageRender(string_buffer, true, true);

//     // Try connect
//     if (!wifi_start(_config.wifiSsid.c_str(), _config.wifiPassword.c_str()))
//     {
//         popWarning("WiFi Connect\n  Failed");
//         return false;
//     }

//     string_buffer = spdlog::fmt_lib::format(" > wifi connected\n get ip: {}\n", wifi_get_ip());
//     onLogPageRender(string_buffer, true, false);

//     return true;
// }
/* ------------------------------- Arduino way ------------------------------ */
bool HAL_VAMeter::connectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect)
{
    // Check config
    if (!checkWifiConfig())
        return false;

    if (reconnect == false && WiFi.status() == WL_CONNECTED)
    {
        spdlog::info("wifi already connected");
        return true;
    }

    std::string string_buffer;

    string_buffer = spdlog::fmt_lib::format(" > try connecting wifi..\n > ssid: {}\n", _config.wifiSsid);
    onLogPageRender(string_buffer, true, true);
    string_buffer += spdlog::fmt_lib::format("pass: {}\n", _config.wifiPassword);
    spdlog::info(string_buffer);

    // Try connect
    WiFi.begin(_config.wifiSsid.c_str(), _config.wifiPassword.c_str());
    bool is_failed = false;
    uint32_t time_count = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(200);
        feedTheDog();

        if (millis() - time_count > 30000)
        {
            is_failed = true;
            break;
        }
    }

    if (is_failed)
    {
        popWarning("WiFi connect\nTimeout");
        return false;
    }

    string_buffer = spdlog::fmt_lib::format(" > wifi connected\n get ip: {}\n", WiFi.localIP().toString().c_str());
    onLogPageRender(string_buffer, true, false);

    return true;
}

bool HAL_VAMeter::disconnectWifi()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        spdlog::info("wifi already disconnected");
        return true;
    }

    WiFi.disconnect();
    return true;
}

std::string HAL_VAMeter::_get_mac()
{
    uint8_t mac[6];
    char string_buffer[18] = {0};
    esp_wifi_get_mac((wifi_interface_t)ESP_IF_WIFI_STA, mac);
    sprintf(string_buffer, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return std::string(string_buffer);
}

std::string HAL_VAMeter::_get_ip()
{
    std::string ip;
    if (WiFi.status() == WL_CONNECTED)
        ip = WiFi.localIP().toString().c_str();
    else
        ip = WiFi.softAPIP().toString().c_str();

    spdlog::info("get ip {}", ip);
    return ip;
}

bool HAL_VAMeter::_start_ap_mode()
{
    spdlog::info("start ap mode");
    disconnectWifi();
    WiFi.softAP(getApWifiSsid().c_str(), emptyString, 1, 0, 4, false);
    spdlog::info("ap mode started");
    return true;
}

bool HAL_VAMeter::_stop_ap_mode()
{
    spdlog::info("stop ap mode");

    if (WiFi.status() == WL_CONNECTED)
    {
        spdlog::info("sta mode, return");
        return true;
    }

    WiFi.softAPdisconnect();
    WiFi.disconnect();
    return true;
}

uint8_t HAL_VAMeter::getApStaNum() { return WiFi.softAPgetStationNum(); }

std::vector<std::string> HAL_VAMeter::_get_wifi_list()
{
    std::vector<std::string> wifi_list;

    // Scan
    spdlog::info("start scan");
    int wifi_num = WiFi.scanNetworks();
    wifi_list.resize(wifi_num);
    for (int i = 0; i < wifi_num; i++)
    {
        wifi_list[i] = WiFi.SSID(i).c_str();
    }
    WiFi.scanDelete();
    spdlog::info("scan done");

    return wifi_list;
}
