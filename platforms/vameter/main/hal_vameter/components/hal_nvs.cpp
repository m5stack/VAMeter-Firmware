/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "nvs_handle.hpp"
// Ref: https://github.com/espressif/esp-idf/blob/v5.1.4/examples/storage/nvs_rw_value_cxx/main/nvs_value_example_main.cpp

void HAL_VAMeter::_nvs_init()
{
    spdlog::info("nvs init");
    _update_boot_count();
}

bool HAL_VAMeter::nvsSet(const char* key, const int32_t& value)
{
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("nvs", NVS_READWRITE, &err);
    if (err != ESP_OK)
    {
        spdlog::error("open nvs handle failed: {}", esp_err_to_name(err));
        return false;
    }

    // Write value
    bool ret = true;
    err = handle->set_item(key, value);
    if (err != ESP_OK)
    {
        spdlog::info("write value failed");
        ret = false;
    }

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    err = handle->commit();

    if (err != ESP_OK)
    {
        spdlog::error("commit value failed");
        ret = false;
    }

    spdlog::info("nvs set {} to {} ok", key, value);
    return ret;
}

int32_t HAL_VAMeter::nvsGet(const char* key)
{
    esp_err_t err;
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("nvs", NVS_READWRITE, &err);
    if (err != ESP_OK)
    {
        spdlog::error("open nvs handle failed: {}", esp_err_to_name(err));
        return 0;
    }

    // Read value
    int32_t value = 0;
    err = handle->get_item(key, value);
    switch (err)
    {
    case ESP_OK:
        spdlog::info("get {} : {}", key, value);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        spdlog::warn("value of {} is not initialized yet", key);
        value = 0;
        break;
    default:
        spdlog::error("read {} failed: {}", key, esp_err_to_name(err));
        value = 0;
        break;
    }

    return value;
}

void HAL_VAMeter::_update_boot_count()
{
    auto boot_count = nvsGet(NVS_KEY_BOOT_COUNT);
    if (boot_count < 10)
    {
        boot_count++;
        nvsSet(NVS_KEY_BOOT_COUNT, boot_count);
    }
}
