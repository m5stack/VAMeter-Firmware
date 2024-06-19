/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <hal/hal.h>

class HAL_VAMeter : public HAL
{
private:
    void _disp_init();
    void _gamepad_init();
    void _encoder_init();
    void _seaker_init();
    void _watch_dog_init();
    void _vabase_init();

    void _i2c_init();
    void _power_monitor_init();
    void _power_monitor_auto_switch_setup();
    void _power_monitor_start_daemon();

    void _fs_init();
    std::vector<std::string> _ls(const std::string& path);
    void _config_check_valid();
    std::string _create_config_json();
    void _backup_config_file();
    void _log_out_system_config();
    void _fs_get_new_rec_file_path(char* recFilePath, size_t bufferSize);
    std::string _fs_get_rec_file_path(const std::string& recordName);

    void _nvs_init();

    std::string _get_mac();
    std::string _get_ip();
    bool _start_ap_mode();
    bool _stop_ap_mode();
    std::vector<std::string> _get_wifi_list();

    void _web_server_page_loading();
    void _web_server_api_loading();
    void _web_server_ws_api_loading();

public:
    inline std::string type() override { return "VAMeter"; }

    inline void init() override
    {
        _gamepad_init();
        _watch_dog_init();
        _fs_init();
        _disp_init();
        _encoder_init();
        _seaker_init();
        _nvs_init();
        _i2c_init();
        _power_monitor_init();
        _vabase_init();
    }

    /* -------------------------------------------------------------------------- */
    /*                             Public api override                            */
    /* -------------------------------------------------------------------------- */
    void reboot() override;

    void feedTheDog() override;

    bool getButton(GAMEPAD::GamePadButton_t button) override;

    void beep(float frequency, uint32_t duration) override;
    void beepStop() override;

    int getEncoderCount() override;
    void resetEncoderCount(int value) override;

    void updatePowerMonitor() override;
    void resetPowerMonitorData() override;
    bool isPowerMonitorInLowCurrentMode() override;
    void powerMonitorCalibration(const float& currentOffset) override;

    void setBaseRelay(bool state) override;
    bool getBaseRelayState() override;

    void loadSystemConfig() override;
    void saveSystemConfig() override;
    void startMscMode() override;
    void stopMscMode() override;
    void factoryReset(OnLogPageRenderCallback_t onLogPageRender) override;

    bool creatVaRecorder(VA_RECORDER::TriggerBase* trigger) override;
    bool isVaRecorderExist() override;
    bool isVaRecorderRecording() override;
    bool isVaRecorderSaving() override;
    bool destroyVaRecorder() override;

    std::vector<std::string> getVaRecordNameList() override;
    std::string getLatestVaRecordName() override;
    VA_RECORDER::Record getVaRecord(const std::string& recordName) override;
    bool deleteVaRecord(const std::string& recordName) override;

    bool connectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect) override;
    bool disconnectWifi() override;
    uint8_t getApStaNum() override;

    OTA_UPGRADE::OtaInfo_t getLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender) override;
    bool upgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl) override;

    bool uploadVaRecordViaEzData(const std::string& recordName, OnLogPageRenderCallback_t onLogPageRender) override;
    std::string getVaRecordEzDataUrl() override;

    bool renderCustomStartupImage() override;
    std::vector<std::string> getStartupImageList() override;

    bool startWebServer(OnLogPageRenderCallback_t onLogPageRender, bool autoWifiMode) override;
    bool stopWebServer() override;
    std::string getSystemConfigUrl() override;

    void baseGroveStartTest() override;
    void baseGroveStopTest() override;
    bool baseGroveGetIoALevel() override;
    bool baseGroveGetIoBLevel() override;

    bool nvsSet(const char* key, const int32_t& value) override;
    int32_t nvsGet(const char* key) override;
    void _update_boot_count();
};
