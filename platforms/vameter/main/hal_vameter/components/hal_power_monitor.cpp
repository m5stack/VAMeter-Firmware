/**
 * @file hal_power_monitor.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-12-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <cstdint>
#include <cstdio>
#include <mooncake.h>
#include <smooth_ui_toolkit.h>
#include "../utils/INA226/src/INA226.h"
#include "hal/gpio_types.h"
#include "hal/hal.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_timer.h>
#include <driver/gpio.h>

// Refs:
// https://github.com/jarzebski/Arduino-INA226/blob/dev/examples/INA226_simple/INA226_simple.ino

static INA226* _ina226_hc = nullptr;
static INA226* _ina226_lc = nullptr;

void HAL_VAMeter::_power_monitor_init()
{
    spdlog::info("power monitor init");

    _ina226_hc = new INA226;
    _ina226_lc = new INA226;

    // High current
    _ina226_hc->begin();
    // 28.4 Hz
    _ina226_hc->configure(
        INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    _ina226_hc->calibrate(0.01, 8.192);

    // Low current
    _ina226_lc->begin(0x41);
    // 3.79 Hz
    _ina226_lc->configure(
        INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    _ina226_lc->calibrate(1.0, 8.192);

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // // Low current
    // _ina226_lc->begin(0x41);
    // // 3.79 Hz
    // _ina226_lc->configure(
    //     INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    // _ina226_lc->calibrate(1.0, 8.192);

    // gpio_reset_pin((gpio_num_t)HAL_PIN_INA266_MODE_CTRL);
    // gpio_set_direction((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, GPIO_MODE_INPUT_OUTPUT);
    // gpio_set_pull_mode((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, GPIO_PULLUP_PULLDOWN);
    // gpio_set_level((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, HAL_INA266_MODE_CTRL_LC_MODE_LEVEL);
    // // gpio_set_level((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, HAL_INA266_MODE_CTRL_HC_MODE_LEVEL);

    // _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    // _canvas->setTextSize(3);
    // _canvas->setFont(&fonts::Font0);
    // _display->setBrightness(255);
    // while (1)
    // {
    //     delay(200);
    //     _canvas->fillScreen(TFT_BLACK);
    //     _canvas->setCursor(0, 0);
    //     _canvas->printf("\n %f\n %f\n\n %f\n %f\n\n %d",
    //                     _ina226_hc->readShuntVoltage(),
    //                     _ina226_hc->readShuntCurrent(),
    //                     _ina226_lc->readShuntVoltage(),
    //                     _ina226_lc->readShuntCurrent(),
    //                     gpio_get_level((gpio_num_t)HAL_PIN_INA266_MODE_CTRL));
    //     _canvas->pushSprite(0, 0);
    //     feedTheDog();
    // }

    // Setup auto switching measure mode
    _power_monitor_auto_switch_setup();
    _power_monitor_start_daemon();

    // /* -------------------------------------------------------------------------- */
    // /*                                    Test                                    */
    // /* -------------------------------------------------------------------------- */
    // _canvas->setTextColor(TFT_WHITE, TFT_BLACK);
    // _canvas->setTextSize(3);
    // _canvas->setFont(&fonts::Font0);
    // _display->setBrightness(255);
    // while (1)
    // {
    //     delay(50);
    //     updatePowerMonitor();
    //     _canvas->fillScreen(TFT_BLACK);
    //     _canvas->setCursor(0, 0);
    //     _canvas->printf("\n %f\n %f\n\n%d",
    //                     getPowerMonitorData().shuntVoltage,
    //                     getPowerMonitorData().shuntCurrent,
    //                     isPowerMonitorInLowCurrentMode());
    //     _canvas->pushSprite(0, 0);
    //     feedTheDog();
    // }

    // _ina226_check_config(*_ina226_hc);
    // printf("\n\n");
    // _ina226_check_config(*_ina226_lc);
    // printf("\n\n");

    // // pinMode(HAL_PIN_INA226_HC_ALERT, INPUT);
    // // pinMode(HAL_PIN_INA226_LC_ALERT, INPUT);
    // while (1)
    // {
    //     // printf("%d\t%d\t%d\t%f\t%f\n",
    //     //        isPowerMonitorInLowCurrentMode(),
    //     //        digitalRead(HAL_PIN_INA226_HC_ALERT),
    //     //        digitalRead(HAL_PIN_INA226_LC_ALERT),
    //     //        _ina226_hc->readShuntVoltage(),
    //     //        _ina226_lc->readShuntVoltage());

    //     printf("is_lc_mode: %d\n", isPowerMonitorInLowCurrentMode());

    //     // printf("%f\t%f\n", _ina226_hc->readShuntVoltage(), _ina226_lc->readShuntVoltage());

    //     delay(20);
    // }
}

/* -------------------------------------------------------------------------- */
/*                      Auto switch measure mode from ISR                     */
/* -------------------------------------------------------------------------- */
static volatile bool _is_low_current_mode = false;
#define _SET_TO_HC_MODE() gpio_set_level((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, HAL_INA266_MODE_CTRL_HC_MODE_LEVEL)
#define _SET_TO_LC_MODE() gpio_set_level((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, HAL_INA266_MODE_CTRL_LC_MODE_LEVEL)

static void IRAM_ATTR _isr_switch_to_hc_mode(void* arg)
{
    *(static_cast<bool*>(arg)) = false;
    _SET_TO_HC_MODE();
}

static void IRAM_ATTR _isr_switch_to_lc_mode(void* arg)
{
    *(static_cast<bool*>(arg)) = true;
    _SET_TO_LC_MODE();
}

void HAL_VAMeter::_power_monitor_auto_switch_setup()
{
    // Switch control pin
    gpio_reset_pin((gpio_num_t)HAL_PIN_INA266_MODE_CTRL);
    gpio_set_direction((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_INA266_MODE_CTRL, GPIO_PULLUP_PULLDOWN);

    // Set alerts
    // Under 0.03A, 0.03A * 0.01R = 0.0003V
    _ina226_hc->enableShuntUnderLimitAlert();
    _ina226_hc->setShuntVoltageLimit(0.0003);
    // Over 0.05A, 0.05A * 1R = 0.05V
    _ina226_lc->enableShuntOverLimitAlert();
    _ina226_lc->setShuntVoltageLimit(0.05);

    // Attach interrupts
    gpio_reset_pin((gpio_num_t)HAL_PIN_INA226_HC_ALERT);
    gpio_set_direction((gpio_num_t)HAL_PIN_INA226_HC_ALERT, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_INA226_HC_ALERT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type((gpio_num_t)HAL_PIN_INA226_HC_ALERT, GPIO_INTR_NEGEDGE);
    gpio_reset_pin((gpio_num_t)HAL_PIN_INA226_LC_ALERT);
    gpio_set_direction((gpio_num_t)HAL_PIN_INA226_LC_ALERT, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)HAL_PIN_INA226_LC_ALERT, GPIO_PULLUP_ONLY);
    gpio_set_intr_type((gpio_num_t)HAL_PIN_INA226_LC_ALERT, GPIO_INTR_NEGEDGE);

    // Handlers
    gpio_install_isr_service(0);
    gpio_isr_handler_add((gpio_num_t)HAL_PIN_INA226_HC_ALERT, _isr_switch_to_lc_mode, (void*)&_is_low_current_mode);
    gpio_isr_handler_add((gpio_num_t)HAL_PIN_INA226_LC_ALERT, _isr_switch_to_hc_mode, (void*)&_is_low_current_mode);

    // Check current mode manually, to provide a default mode state
    // 3 times same result to break the loop
    int same_times = 0;
    int try_times = 0;
    while (1)
    {
        if (_ina226_hc->readShuntCurrent() >= 0.05)
        {
            if (_is_low_current_mode)
                same_times = 0;
            _is_low_current_mode = false;
        }
        else if (_ina226_hc->readShuntCurrent() <= 0.03)
        {
            if (!_is_low_current_mode)
                same_times = 0;
            _is_low_current_mode = true;
        }

        same_times++;
        try_times++;
        if (same_times > 3 || try_times > 10)
            break;
        delay(10);
    }
    if (try_times > 10)
        popFatalError("Current not stable");

    // Update mode
    if (_is_low_current_mode)
        _SET_TO_LC_MODE();
    else
        _SET_TO_HC_MODE();

    spdlog::info("get default state: {} mode", _is_low_current_mode ? "lc" : "hc");
}

/* -------------------------------------------------------------------------- */
/*                   A daemon to handle monitor data update                   */
/* -------------------------------------------------------------------------- */
static SemaphoreHandle_t _pm_data_handle_mutex;
static POWER_MONITOR::PMData_t* _pm_data_daemon = nullptr;
static float _pm_data_daemon_current_offset = 0.0f;

// Expose borrow apis to other daemon
POWER_MONITOR::PMData_t* _borrow_pm_data_daemon()
{
    xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);
    return _pm_data_daemon;
}
void _return_pm_data_daemon() { xSemaphoreGive(_pm_data_handle_mutex); }

// Expose a crrent offset setting api to hal layer
void _set_pm_data_daemon_current_offset(const float& offset)
{
    xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);
    _pm_data_daemon_current_offset = offset;
    xSemaphoreGive(_pm_data_handle_mutex);
}

static void _handle_basic_data_update()
{
    _pm_data_daemon->busVoltage = _ina226_hc->readBusVoltage();
    _pm_data_daemon->busPower = _ina226_hc->readBusPower();
    if (_is_low_current_mode)
    {
        _pm_data_daemon->shuntVoltage = _ina226_lc->readShuntVoltage();

        // It should be the same, but readShuntCurrent() not
        // _pm_data_daemon->shuntCurrent = _ina226_lc->readShuntCurrent();
        _pm_data_daemon->shuntCurrent = _pm_data_daemon->shuntVoltage;

        // Apply calibration
        _pm_data_daemon->shuntCurrent -= _pm_data_daemon_current_offset;
        if (_pm_data_daemon->shuntCurrent < 0.0f)
            _pm_data_daemon->shuntCurrent = 0.0f;
    }
    else
    {
        _pm_data_daemon->shuntVoltage = _ina226_hc->readShuntVoltage();
        _pm_data_daemon->shuntCurrent = _ina226_hc->readShuntCurrent();
    }
}

static constexpr int _pm_data_daemon_update_interval = 35;
static constexpr size_t _avg_sample_num_in_second = 1000 / _pm_data_daemon_update_interval;
static constexpr size_t _avg_sample_num_in_min = 60 * _avg_sample_num_in_second;

class AvgCulculator : public SmoothUIToolKit::RingBuffer<float, _avg_sample_num_in_min>
{
private:
    struct Data_t
    {
        float avg_min = 0.0f;
        float avg_second = 0.0f;
        float avg_total = 0.0f;

        size_t total_sample_num = 0;
        size_t current_sample_num = 0;
        size_t peek_index = 0;
    };
    Data_t _data;

public:
    void addSample(const float& newSample)
    {
        // Push new sample
        put(newSample);

        // Reset
        _data.avg_min = 0.0f;
        _data.avg_second = 0.0f;
        _data.current_sample_num = 0;
        _data.peek_index = readIndex();

        // Avg in min
        while (_data.peek_index != writeIndex())
        {
            _data.avg_min += rawBuffer()[_data.peek_index];
            _data.current_sample_num++;
            _data.peek_index = (_data.peek_index + 1) % size();
        }
        _data.avg_min = _data.avg_min / _data.current_sample_num;

        // Avg in second
        if (_data.current_sample_num <= _avg_sample_num_in_second)
            _data.avg_second = _data.avg_min;
        else
        {
            // Jump to latest second sample
            _data.peek_index = (readIndex() + (_data.current_sample_num - _avg_sample_num_in_second)) % size();
            while (_data.peek_index != writeIndex())
            {
                _data.avg_second += rawBuffer()[_data.peek_index];
                _data.peek_index = (_data.peek_index + 1) % size();
            }
            _data.avg_second = _data.avg_second / _avg_sample_num_in_second;
        }

        // Avg total
        _data.total_sample_num++;
        _data.avg_total = _data.avg_total * (_data.total_sample_num - 1) + newSample;
        _data.avg_total = _data.avg_total / _data.total_sample_num;
    }

    const float& getAvgMin() { return _data.avg_min; }
    const float& getAvgSecond() { return _data.avg_second; }
    const float& getAvgTotal() { return _data.avg_total; }
};
static AvgCulculator* _avg_culculator = nullptr;

static void _handle_avg_update()
{
    if (_avg_culculator == nullptr)
        _avg_culculator = new AvgCulculator;

    _avg_culculator->addSample(_pm_data_daemon->shuntCurrent);
    _pm_data_daemon->currentAvgMin = _avg_culculator->getAvgMin();
    _pm_data_daemon->currentAvgSecond = _avg_culculator->getAvgSecond();
    _pm_data_daemon->currentAvgTotal = _avg_culculator->getAvgTotal();
}

static void _handle_peak_and_min_update()
{
    if (_pm_data_daemon->shuntCurrent > _pm_data_daemon->currentPeak)
        _pm_data_daemon->currentPeak = _pm_data_daemon->shuntCurrent;
    if (_pm_data_daemon->shuntCurrent < _pm_data_daemon->currentMin)
        _pm_data_daemon->currentMin = _pm_data_daemon->shuntCurrent;
}

static void _handle_capacity_and_energy_update()
{
    _pm_data_daemon->capacity += (_pm_data_daemon->shuntCurrent * _pm_data_daemon_update_interval) / (60 * 60 * 1000);
    _pm_data_daemon->energy +=
        (_pm_data_daemon->shuntCurrent * _pm_data_daemon->busVoltage * _pm_data_daemon_update_interval) / (60 * 60 * 1000);
}

static void _handle_time_tag_update(uint32_t& timeCountStart)
{
    _pm_data_daemon->time = esp_timer_get_time() / 1000 - timeCountStart;
}

static uint32_t _time_count_start = 0;
static void _power_monitor_daemon(void* param)
{
    _time_count_start = esp_timer_get_time() / 1000;

    _pm_data_daemon->currentPeak = -23.3f;
    _pm_data_daemon->currentMin = 23.3f;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(_pm_data_daemon_update_interval));

        // Update pm data
        xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);

        _handle_basic_data_update();
        _handle_avg_update();
        _handle_peak_and_min_update();
        _handle_capacity_and_energy_update();
        _handle_time_tag_update(_time_count_start);

        xSemaphoreGive(_pm_data_handle_mutex);
    }
    vTaskDelete(NULL);
}

void HAL_VAMeter::_power_monitor_start_daemon()
{
    // Create data handle mutex
    _pm_data_handle_mutex = xSemaphoreCreateMutex();

    // Apply calibration
    powerMonitorCalibration(_config.currentOffset);

    // Create task
    _pm_data_daemon = new POWER_MONITOR::PMData_t;
    _ina226_hc = _ina226_hc;
    _ina226_lc = _ina226_lc;
    xTaskCreate(_power_monitor_daemon, "PMD", 4028, NULL, 1, NULL);
}

/* -------------------------------------------------------------------------- */
/*                          Public data fetching api                          */
/* -------------------------------------------------------------------------- */
void HAL_VAMeter::updatePowerMonitor()
{
    // Sync pm data to daemon
    xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);

    // Copy
    _pm_data.busVoltage = _pm_data_daemon->busVoltage;
    _pm_data.busPower = _pm_data_daemon->busPower;
    _pm_data.shuntVoltage = _pm_data_daemon->shuntVoltage;
    _pm_data.shuntCurrent = _pm_data_daemon->shuntCurrent;
    _pm_data.currentAvgSecond = _pm_data_daemon->currentAvgSecond;
    _pm_data.currentAvgMin = _pm_data_daemon->currentAvgMin;
    _pm_data.currentAvgTotal = _pm_data_daemon->currentAvgTotal;
    _pm_data.currentPeak = _pm_data_daemon->currentPeak;
    _pm_data.currentMin = _pm_data_daemon->currentMin;
    _pm_data.capacity = _pm_data_daemon->capacity;
    _pm_data.energy = _pm_data_daemon->energy;
    _pm_data.time = _pm_data_daemon->time;

    xSemaphoreGive(_pm_data_handle_mutex);
}

void HAL_VAMeter::resetPowerMonitorData()
{
    xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);

    // Reset
    _pm_data_daemon->currentAvgSecond = 0.0f;
    _pm_data_daemon->currentAvgMin = 0.0f;
    _pm_data_daemon->currentAvgTotal = 0.0f;
    _pm_data_daemon->currentPeak = -23.3f;
    _pm_data_daemon->currentMin = 23.3f;
    _pm_data_daemon->capacity = 0.0f;
    _pm_data_daemon->energy = 0.0f;
    _pm_data_daemon->time = 0;
    _time_count_start = esp_timer_get_time() / 1000;

    xSemaphoreGive(_pm_data_handle_mutex);
}

bool HAL_VAMeter::isPowerMonitorInLowCurrentMode() { return _is_low_current_mode; }

void HAL_VAMeter::powerMonitorCalibration(const float& currentOffset)
{
    xSemaphoreTake(_pm_data_handle_mutex, portMAX_DELAY);
    _pm_data_daemon_current_offset = currentOffset;
    xSemaphoreGive(_pm_data_handle_mutex);
}
