/**
 * @file hal.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-11-04
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <functional>
#include <vector>
#ifndef ESP_PLATFORM
#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>
#include <cstdint>
#include <iostream>
#include <string>
#include "utils/lgfx_fx/lgfx_fx.h"
#include "types.h"
#else
#include "utils/lgfx_fx/lgfx_fx.h"
#include <LovyanGFX.hpp>
#include "types.h"
#endif

/**
 * @brief Provide a dingleton to abstract hardware methods
 * 1) Inherit and override methods to create a specific hal
 * 2) Use HAL::Inject() to inject your hal
 * 3) Use HAL:Get() to get this hal wherever you want
 */
class HAL
{
private:
    static HAL* _hal;

public:
    /**
     * @brief Get HAL instance
     *
     * @return HAL*
     */
    static HAL* Get();

    /**
     * @brief Check if HAL is valid
     *
     * @return true
     * @return false
     */
    static bool Check();

    /**
     * @brief HAL injection, init() will be called here
     *
     * @param hal
     * @return true
     * @return false
     */
    static bool Inject(HAL* hal);

    /**
     * @brief Destroy HAL instance
     *
     */
    static void Destroy();

    /**
     * @brief Base class
     *
     */
public:
    HAL() : _display(nullptr), _canvas(nullptr) {}
    virtual ~HAL() {}

    static std::string Type() { return Get()->type(); }
    virtual std::string type() { return "Base"; }

    static std::string Version() { return Get()->version(); }
    virtual std::string version() { return APP_VERSION; }

    static std::string CompileDate() { return Get()->compileDate(); }
    virtual std::string compileDate() { return __DATE__; }

    virtual void init() {}

    /**
     * @brief Components
     *
     */
protected:
    LGFX_Device* _display;
    LGFX_SpriteFx* _canvas;
    time_t _time_buffer;
    POWER_MONITOR::PMData_t _pm_data;
    CONFIG::SystemConfig_t _config;

    /* -------------------------------------------------------------------------- */
    /*                                   Display                                  */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Display device
     *
     * @return LGFX_Device*
     */
    static LGFX_Device* GetDisplay() { return Get()->_display; }

    /**
     * @brief Full screen canvas (sprite)
     *
     * @return LGFX_SpriteFx*
     */
    static LGFX_SpriteFx* GetCanvas() { return Get()->_canvas; }

    /**
     * @brief Push framebuffer
     *
     */
    static void CanvasUpdate() { Get()->canvasUpdate(); }
    virtual void canvasUpdate() { GetCanvas()->pushSprite(0, 0); }

    /**
     * @brief Render fps panel
     *
     */
    static void RenderFpsPanel() { Get()->renderFpsPanel(); }
    virtual void renderFpsPanel();

    /**
     * @brief Pop error message and wait reboot
     *
     * @param msg
     */
    static void PopFatalError(std::string msg) { Get()->popFatalError(msg); }
    virtual void popFatalError(std::string msg);

    /**
     * @brief Pop warning message and wait continue
     *
     * @param msg
     */
    static void PopWarning(std::string msg) { Get()->popWarning(msg); }
    virtual void popWarning(std::string msg);

    /**
     * @brief Pop success message and wait continue
     *
     * @param msg
     */
    static void PopSuccess(std::string msg, bool showSuccessLabel = true) { Get()->popSuccess(msg, showSuccessLabel); }
    virtual void popSuccess(std::string msg, bool showSuccessLabel = true);

    /* -------------------------------------------------------------------------- */
    /*                                   System                                   */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Delay(ms)
     *
     * @param milliseconds
     */
    static void Delay(unsigned long milliseconds) { Get()->delay(milliseconds); }
    virtual void delay(unsigned long milliseconds) { lgfx::delay(milliseconds); }

    /**
     * @brief Get the number of milliseconds passed since boot
     *
     * @return unsigned long
     */
    static unsigned long Millis() { return Get()->millis(); }
    virtual unsigned long millis() { return lgfx::millis(); }

    /**
     * @brief Power off
     *
     */
    static void PowerOff() { Get()->powerOff(); }
    virtual void powerOff() {}

    /**
     * @brief Reboot
     *
     */
    static void Reboot() { Get()->reboot(); }
    virtual void reboot() {}

    /**
     * @brief Set RTC time
     *
     * @param dateTime
     */
    static void SetSystemTime(tm dateTime) { return Get()->setSystemTime(dateTime); }
    virtual void setSystemTime(tm dateTime) {}

    /**
     * @brief Get local time(wrap of localtime())
     *
     * @return tm*
     */
    static tm* GetLocalTime() { return Get()->getLocalTime(); }
    virtual tm* getLocalTime();

    /**
     * @brief Reset system watch dog
     *
     */
    static void FeedTheDog() { Get()->feedTheDog(); }
    virtual void feedTheDog() {}

    /**
     * @brief Factory reset device
     *
     * @param onLogPageRender
     */
    static void FactoryReset(OnLogPageRenderCallback_t onLogPageRender) { Get()->factoryReset(onLogPageRender); }
    virtual void factoryReset(OnLogPageRenderCallback_t onLogPageRender) {}

    /* -------------------------------------------------------------------------- */
    /*                                  MSC Mode                                  */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Start MSC mode to expose fs as a USB disk
     *
     */
    static void StartMscMode() { Get()->startMscMode(); }
    virtual void startMscMode() {}

    /**
     * @brief Stop MSC mode
     *
     */
    static void StopMscMode() { Get()->stopMscMode(); }
    virtual void stopMscMode() {}

    /* -------------------------------------------------------------------------- */
    /*                                Startup image                               */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Render custom startup image
     *
     * @return true
     * @return false
     */
    static bool RenderCustomStartupImage() { return Get()->renderCustomStartupImage(); }
    virtual bool renderCustomStartupImage() { return false; }

    static std::vector<std::string> GetStartupImageList() { return Get()->getStartupImageList(); }
    virtual std::vector<std::string> getStartupImageList() { return std::vector<std::string>(); }

    /* -------------------------------------------------------------------------- */
    /*                                System config                               */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Load system config from fs
     *
     */
    static void LoadSystemConfig() { Get()->loadSystemConfig(); }
    virtual void loadSystemConfig() {}

    /**
     * @brief Save system config to fs
     *
     */
    static void SaveSystemConfig() { Get()->saveSystemConfig(); }
    virtual void saveSystemConfig() {}

    /**
     * @brief Get system config
     *
     * @return CONFIG::SystemConfig_t&
     */
    static CONFIG::SystemConfig_t& GetSystemConfig() { return Get()->_config; }

    /**
     * @brief Set system config
     *
     * @param cfg
     */
    static void SetSystemConfig(CONFIG::SystemConfig_t cfg) { Get()->_config = cfg; }

    /**
     * @brief Apply system config to device
     *
     */
    static void ApplySystemConfig() { Get()->applySystemConfig(); }
    virtual void applySystemConfig() {}

    /* -------------------------------------------------------------------------- */
    /*                                   Buzzer                                   */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Buzzer beep
     *
     * @param frequency
     * @param duration
     */
    static void Beep(float frequency, uint32_t duration = 4294967295U) { Get()->beep(frequency, duration); }
    virtual void beep(float frequency, uint32_t duration) {}

    /**
     * @brief Stop buzzer beep
     *
     */
    static void BeepStop() { Get()->beepStop(); }
    virtual void beepStop() {}

    /* -------------------------------------------------------------------------- */
    /*                                   Gamepad                                  */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Get button state, 获取按键状态
     *
     * @param button
     * @return true Pressing, 按下
     * @return false Released, 松开
     */
    static bool GetButton(GAMEPAD::GamePadButton_t button) { return Get()->getButton(button); }
    virtual bool getButton(GAMEPAD::GamePadButton_t button) { return false; }

    /**
     * @brief Get any button state, 获取任意按键状态
     *
     * @return true Pressing, 按下
     * @return false Released, 松开
     */
    static bool GetAnyButton() { return Get()->getAnyButton(); }
    virtual bool getAnyButton();

    /* -------------------------------------------------------------------------- */
    /*                                   Encoder                                  */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Get encoder count
     *
     * @return int
     */
    static int GetEncoderCount() { return Get()->getEncoderCount(); }
    virtual int getEncoderCount() { return 0; }

    /**
     * @brief Reset encoder count
     *
     */
    static void ResetEncoderCount(int value = 0) { Get()->resetEncoderCount(value); }
    virtual void resetEncoderCount(int value) {}

    /* -------------------------------------------------------------------------- */
    /*                                  Touchpad                                  */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Update touch
     *
     */
    static void UpdateTouch() { Get()->updateTouch(); }
    virtual void updateTouch() {}

    /**
     * @brief Is touching
     *
     * @return int
     */
    static int IsTouching() { return Get()->isTouching(); }
    virtual bool isTouching() { return false; }

    /**
     * @brief Get touch point
     *
     * @return TOUCH::Point_t
     */
    static TOUCH::Point_t GetTouchPoint() { return Get()->getTouchPoint(); }
    virtual TOUCH::Point_t getTouchPoint() { return {-1, -1}; }

    /* -------------------------------------------------------------------------- */
    /*                                Power monitor                               */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Update power monitor data
     *
     */
    static void UpdatePowerMonitor() { Get()->updatePowerMonitor(); }
    virtual void updatePowerMonitor() {}

    /**
     * @brief Get power monitor data
     *
     * @return const POWER_MONITOR::PMData_t&
     */
    static const POWER_MONITOR::PMData_t& GetPowerMonitorData() { return Get()->getPowerMonitorData(); }
    virtual const POWER_MONITOR::PMData_t& getPowerMonitorData() { return _pm_data; }

    /**
     * @brief Reset power monitor data
     *
     */
    static void ResetPowerMonitorData() { Get()->resetPowerMonitorData(); }
    virtual void resetPowerMonitorData() {}

    /**
     * @brief Get if is power monitor is in low current measure mode
     *
     * @return true
     * @return false
     */
    static bool IsPowerMonitorInLowCurrentMode() { return Get()->isPowerMonitorInLowCurrentMode(); }
    virtual bool isPowerMonitorInLowCurrentMode() { return true; }

    /**
     * @brief Calibration
     *
     * @param currentOffset
     */
    static void PowerMonitorCalibration(const float& currentOffset) { Get()->powerMonitorCalibration(currentOffset); }
    virtual void powerMonitorCalibration(const float& currentOffset) {}

    /**
     * @brief Helper method to get auto unit adaptaed value
     *
     * @param voltage
     * @return POWER_MONITOR::UnitAdaptatedData_t
     */
    static POWER_MONITOR::UnitAdaptatedData_t GetUnitAdaptatedVoltage(const float& voltage);
    static POWER_MONITOR::UnitAdaptatedData_t GetUnitAdaptatedCurrent(const float& current);
    static POWER_MONITOR::UnitAdaptatedData_t GetUnitAdaptatedPower(const float& power);
    static POWER_MONITOR::UnitAdaptatedData_t GetUnitAdaptatedCapacity(const float& capacity);
    static POWER_MONITOR::UnitAdaptatedData_t GetUnitAdaptatedEnergy(const float& energy);

    /* -------------------------------------------------------------------------- */
    /*                                 Base relay                                 */
    /* -------------------------------------------------------------------------- */
public:
    /**
     * @brief Set the state of base relay
     *
     * @param state true: close, false: open
     */
    static void SetBaseRelay(bool state) { Get()->setBaseRelay(state); }
    virtual void setBaseRelay(bool state) {}

    /**
     * @brief Get the current state of base relay
     *
     * @return true
     * @return false
     */
    static bool GetBaseRelayState() { return Get()->getBaseRelayState(); }
    virtual bool getBaseRelayState() { return false; }

    /* -------------------------------------------------------------------------- */
    /*                                 Base grove                                 */
    /* -------------------------------------------------------------------------- */
public:
    static void BaseGroveStartTest() { Get()->baseGroveStartTest(); }
    virtual void baseGroveStartTest() {}

    static void BaseGroveStopTest() { Get()->baseGroveStopTest(); }
    virtual void baseGroveStopTest() {}

    static bool BaseGroveGetIoALevel() { return Get()->baseGroveGetIoALevel(); }
    virtual bool baseGroveGetIoALevel() { return true; }

    static bool BaseGroveGetIoBLevel() { return Get()->baseGroveGetIoBLevel(); }
    virtual bool baseGroveGetIoBLevel() { return true; }

    /* -------------------------------------------------------------------------- */
    /*                                 VA Recoder                                 */
    /* -------------------------------------------------------------------------- */
public:
    static bool CreatVaRecorder(VA_RECORDER::TriggerBase* trigger) { return Get()->creatVaRecorder(trigger); }
    virtual bool creatVaRecorder(VA_RECORDER::TriggerBase* trigger) { return false; }

    static bool IsVaRecorderExist() { return Get()->isVaRecorderExist(); }
    virtual bool isVaRecorderExist() { return false; }

    static bool IsVaRecorderRecording() { return Get()->isVaRecorderRecording(); }
    virtual bool isVaRecorderRecording() { return false; }

    static bool IsVaRecorderSaving() { return Get()->isVaRecorderSaving(); }
    virtual bool isVaRecorderSaving() { return false; }

    static bool DestroyVaRecorder() { return Get()->destroyVaRecorder(); }
    virtual bool destroyVaRecorder() { return false; }

    /* -------------------------------------------------------------------------- */
    /*                                  VA Record                                 */
    /* -------------------------------------------------------------------------- */
public:
    static std::vector<std::string> GetVaRecordNameList() { return Get()->getVaRecordNameList(); }
    virtual std::vector<std::string> getVaRecordNameList() { return std::vector<std::string>(); }

    static std::string GetLatestVaRecordName() { return Get()->getLatestVaRecordName(); }
    virtual std::string getLatestVaRecordName() { return ""; }

    static VA_RECORDER::Record GetVaRecord(const std::string& recordName) { return Get()->getVaRecord(recordName); }
    virtual VA_RECORDER::Record getVaRecord(const std::string& recordName) { return VA_RECORDER::Record(); }

    static bool DeleteVaRecord(const std::string& recordName) { return Get()->deleteVaRecord(recordName); }
    virtual bool deleteVaRecord(const std::string& recordName) { return false; }

    static VA_RECORDER::Record GetLatestVaRecord() { return Get()->getVaRecord(Get()->GetLatestVaRecordName()); }

    /* -------------------------------------------------------------------------- */
    /*                                   Network                                  */
    /* -------------------------------------------------------------------------- */
public:
    static bool CheckWifiConfig() { return Get()->checkWifiConfig(); }
    virtual bool checkWifiConfig();

    static bool ConnectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect = false)
    {
        return Get()->connectWifi(onLogPageRender, reconnect);
    }
    virtual bool connectWifi(OnLogPageRenderCallback_t onLogPageRender, bool reconnect) { return false; }

    virtual bool DisconnectWifi() { return Get()->disconnectWifi(); }
    virtual bool disconnectWifi() { return true; }

    static bool IsWifiInApMode() { return Get()->isWifiInApMode(); }
    virtual bool isWifiInApMode() { return true; }

    static std::string GetApWifiSsid() { return Get()->getApWifiSsid(); }
    virtual std::string getApWifiSsid() { return "M5-VAMeter-WiFi"; }

    static uint8_t GetApStaNum() { return Get()->getApStaNum(); }
    virtual uint8_t getApStaNum() { return 0; }

    /* -------------------------------------------------------------------------- */
    /*                                   EzData                                   */
    /* -------------------------------------------------------------------------- */
public:
    static bool UploadVaRecordViaEzData(const std::string& recordName, OnLogPageRenderCallback_t onLogPageRender)
    {
        return Get()->uploadVaRecordViaEzData(recordName, onLogPageRender);
    }
    virtual bool uploadVaRecordViaEzData(const std::string& recordName, OnLogPageRenderCallback_t onLogPageRender)
    {
        return false;
    }

    static std::string GetVaRecordEzDataUrl() { return Get()->getVaRecordEzDataUrl(); }
    virtual std::string getVaRecordEzDataUrl() { return "https://ezdatatatatattatat/vamter/1233211234567"; }

    /* -------------------------------------------------------------------------- */
    /*                                     OTA                                    */
    /* -------------------------------------------------------------------------- */
public:
    static OTA_UPGRADE::OtaInfo_t GetLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender)
    {
        return Get()->getLatestFirmwareInfoViaOta(onLogPageRender);
    }
    virtual OTA_UPGRADE::OtaInfo_t getLatestFirmwareInfoViaOta(OnLogPageRenderCallback_t onLogPageRender)
    {
        return OTA_UPGRADE::OtaInfo_t();
    }

    static bool UpgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl)
    {
        return Get()->upgradeFirmwareViaOta(onLogPageRender, firmwareUrl);
    }
    virtual bool upgradeFirmwareViaOta(OnLogPageRenderCallback_t onLogPageRender, const std::string& firmwareUrl)
    {
        return false;
    }

    /* -------------------------------------------------------------------------- */
    /*                                 Web server                                 */
    /* -------------------------------------------------------------------------- */
public:
    static bool StartWebServer(OnLogPageRenderCallback_t onLogPageRender, bool autoWifiMode = false)
    {
        return Get()->startWebServer(onLogPageRender, autoWifiMode);
    }
    virtual bool startWebServer(OnLogPageRenderCallback_t onLogPageRender, bool autoWifiMode) { return false; }

    static bool StopWebServer() { return Get()->stopWebServer(); }
    virtual bool stopWebServer() { return true; }

    static std::string GetSystemConfigUrl() { return Get()->getSystemConfigUrl(); }
    virtual std::string getSystemConfigUrl() { return "http://192.168.4.1/syscfg"; }
};
