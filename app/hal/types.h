/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <smooth_ui_toolkit.h>

/* -------------------------------------------------------------------------- */
/*                               Gamepad button                               */
/* -------------------------------------------------------------------------- */
namespace GAMEPAD
{
    enum GamePadButton_t
    {
        BTN_START = 0,
        BTN_SELECT,
        BTN_UP,
        BTN_LEFT,
        BTN_RIGHT,
        BTN_DOWN,
        BTN_X,
        BTN_Y,
        BTN_A,
        BTN_B,
        BTN_LEFT_STICK,
        GAMEPAD_BUTTON_NUM,
    };
} // namespace GAMEPAD

/* -------------------------------------------------------------------------- */
/*                                     IMU                                    */
/* -------------------------------------------------------------------------- */
namespace IMU
{
    struct ImuData_t
    {
        float accelX;
        float accelY;
        float accelZ;
    };
} // namespace IMU

/* -------------------------------------------------------------------------- */
/*                                  Touchpad                                  */
/* -------------------------------------------------------------------------- */
namespace TOUCH
{
    /**
     * @brief Touch point
     *
     */
    struct Point_t
    {
        int x;
        int y;
    };
} // namespace TOUCH

/* -------------------------------------------------------------------------- */
/*                                Power monitor                               */
/* -------------------------------------------------------------------------- */
namespace POWER_MONITOR
{
    struct PMData_t
    {
        // Basic
        float busVoltage = 0.0f;
        float busPower = 0.0f;
        float shuntVoltage = 0.0f;
        float shuntCurrent = 0.0f;

        // From daemon
        float currentAvgSecond = 0.0f;
        float currentAvgMin = 0.0f;
        float currentAvgTotal = 0.0f;
        float currentPeak = 0.0f;
        float currentMin = 0.0f;
        float capacity = 0.0f;
        float energy = 0.0f;
        // char time[12];
        uint32_t time = 0;
    };

    struct UnitAdaptatedData_t
    {
        std::string value;
        std::string unit;
    };
}; // namespace POWER_MONITOR

/* -------------------------------------------------------------------------- */
/*                                Localization                                */
/* -------------------------------------------------------------------------- */
enum LocaleCode_t
{
    locale_code_en = 0,
    locale_code_cn,
    locale_code_jp,
};

/* -------------------------------------------------------------------------- */
/*                                System config                               */
/* -------------------------------------------------------------------------- */
namespace CONFIG
{
    // Default config
    struct SystemConfig_t
    {
        int brightness = 255;
        int orientation = 0;
        bool beepOn = true;
        bool highRefreshRate = true;
        bool reverseEncoder = false;
        LocaleCode_t localeCode = locale_code_en;
        float currentOffset = 0.0f;
        std::string wifiSsid;
        std::string wifiPassword;
        std::string startupImage;
    };

#define PASS_STARTUP_IMAGE_TAG "__pass"
#define DEFAULT_STARTUP_IMAGE_DARK_TAG "__dark"
#define DEFAULT_STARTUP_IMAGE_LIGHT_TAG "__light"
} // namespace CONFIG

/* -------------------------------------------------------------------------- */
/*                                 VA Recoder                                 */
/* -------------------------------------------------------------------------- */
namespace VA_RECORDER
{
    struct RecordData_t
    {
        float voltage = 0.0f;
        float current = 0.0f;

        RecordData_t() = default;
        RecordData_t(const float& voltage, const float& current)
        {
            this->voltage = voltage;
            this->current = current;
        }
    };

    typedef SmoothUIToolKit::RingBuffer<RecordData_t, 1> PreTriggerBuffer;
    typedef std::vector<RecordData_t> Record;

    class TriggerBase
    {
    protected:
        struct Data_t
        {
            float threshold = 1.0f;
            uint32_t record_time = 10000;
            uint32_t sample_interval = 40;
            bool is_voltage_channel = true;
            bool has_threshold = false;
        };
        Data_t _data;

    public:
        virtual ~TriggerBase() {}

        virtual size_t preTriggerBufferSize() { return 0; }
        virtual bool onCheck(PreTriggerBuffer* preTriggerBuffer) { return true; }

        inline const uint32_t& getRecordTime() { return _data.record_time; }
        inline void setRecordTime(const uint32_t recordTime) { _data.record_time = recordTime; }

        inline const float& getThreshold() { return _data.threshold; }
        inline void setThreshold(const float& threshold) { _data.threshold = threshold; }

        inline const uint32_t& getSampleInterval() { return _data.sample_interval; }
        inline void setSampleInterval(const uint32_t sampleInterval) { _data.sample_interval = sampleInterval; }

        /**
         * @brief
         *
         * @return true V
         * @return false A
         */
        inline const bool& getTriggerChannel() { return _data.is_voltage_channel; }
        inline void setTriggerChannel(const bool& isVoltageChannel) { _data.is_voltage_channel = isVoltageChannel; }

        inline const bool& getHasThreshold() { return _data.has_threshold; }
        inline void setHasThreshold(const bool& hasThreshold) { _data.has_threshold = hasThreshold; }
    };
} // namespace VA_RECORDER

/* -------------------------------------------------------------------------- */
/*                                  OTA info                                  */
/* -------------------------------------------------------------------------- */
namespace OTA_UPGRADE
{
    struct OtaInfo_t
    {
        bool getInfoFailed = false;
        bool upgradeAvailable = false;
        bool needUsbFlashing = false;
        std::string latestVersion;
        std::string firmwareUrl;
        std::string description;
    };
}; // namespace OTA_UPGRADE

/* -------------------------------------------------------------------------- */
/*                                    MISC                                    */
/* -------------------------------------------------------------------------- */
typedef std::function<void(const std::string& log, bool pushScreen, bool clear)> OnLogPageRenderCallback_t;

#define APP_VERSION "V1.2.0"

/* -------------------------------------------------------------------------- */
/*                                  NVS keys                                  */
/* -------------------------------------------------------------------------- */
#define NVS_KEY_APP_HISTORY "app_history"
#define NVS_KEY_BOOT_COUNT "boot_count"
