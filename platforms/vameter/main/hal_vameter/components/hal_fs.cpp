/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <string>
#include <ArduinoJson.h>
#include <dirent.h>
extern "C"
{
#include "../utils/wear_levelling/wear_levelling.h"
#include "../utils/tusb_msc/tusb_msc.h"
}

void HAL_VAMeter::_fs_init()
{
    spdlog::info("fs init");

    if (!fs_init())
    {
        _disp_init();
        popFatalError("Fs init failed");
    }

    loadSystemConfig();
    _log_out_system_config();
}

void HAL_VAMeter::_log_out_system_config()
{
    spdlog::info("system config:");
    spdlog::info(" - brightness: {}", _config.brightness);
    spdlog::info(" - rotation: {}", _config.orientation);
    spdlog::info(" - beep on: {}", _config.beepOn);
    spdlog::info(" - wifi ssid: {}", _config.wifiSsid);
    spdlog::info(" - wifi pass: {}", _config.wifiPassword);
    spdlog::info(" - reverse encoder: {}", _config.reverseEncoder);
    spdlog::info(" - locale code: {}", (int)_config.localeCode);
    spdlog::info(" - currrnt offset: {:0.7f}", _config.currentOffset);
    spdlog::info(" - startup image: {}", _config.startupImage);
}

/* -------------------------------------------------------------------------- */
/*                               Config realated                              */
/* -------------------------------------------------------------------------- */
static const char* _system_config_path = "/spiflash/system_config.json";
static const char* _system_config_backup_path = "/spiflash/system_config.json.bk";

void HAL_VAMeter::_config_check_valid()
{
    spdlog::info("check config valid");

    if (_config.brightness > 255 || _config.brightness < 0)
        _config.brightness = 255;

    if (_config.orientation > 3 || _config.orientation < 0)
        _config.orientation = 0;
}

void HAL_VAMeter::loadSystemConfig()
{
    spdlog::info("load config from fs");

    // Open config
    FILE* config_file = fopen(_system_config_path, "rb");
    if (config_file == NULL)
    {
        spdlog::warn("{} not exist", _system_config_path);
        saveSystemConfig();
        _backup_config_file();
        return;
    }

    // Copy content
    char* file_content = 0;
    long file_length = 0;
    fseek(config_file, 0, SEEK_END);
    file_length = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);
    file_content = (char*)malloc(file_length);

    // If shit happened
    if (!file_content)
    {
        fclose(config_file);

        spdlog::warn("malloc failed, size: {}", file_length);
        spdlog::info("try config backup");

        config_file = fopen(_system_config_backup_path, "rb");
        if (config_file == NULL)
        {
            spdlog::info("open backup failed, recreate config file");
            saveSystemConfig();
            _backup_config_file();
            return;
        }
    }
    else
    {
        fread(file_content, 1, file_length, config_file);
        fclose(config_file);
    }

    // Parse
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file_content);
    if (error != DeserializationError::Ok)
    {
        free(file_content);
        // spdlog::error("json invaild");

        _disp_init();
        popWarning("Config JSON\nInvaild");

        saveSystemConfig();
        return;
    }

    // Copy
    _config.brightness = doc["brightness"];
    _config.orientation = doc["orientation"];
    _config.beepOn = doc["beepOn"];
    _config.highRefreshRate = doc["highRefreshRate"];
    _config.reverseEncoder = doc["reverseEncoder"];
    _config.currentOffset = doc["currentOffset"];
    _config.wifiSsid = doc["wifiSsid"].as<std::string>();
    _config.wifiPassword = doc["wifiPassword"].as<std::string>();
    _config.startupImage = doc["startupImage"].as<std::string>();

    int locale_code = doc["localeCode"];
    if (locale_code <= (int)locale_code_jp && locale_code >= 0)
        _config.localeCode = (LocaleCode_t)locale_code;

    free(file_content);
    spdlog::info("done");

    _config_check_valid();
}

std::string HAL_VAMeter::_create_config_json()
{
    // Create json
    JsonDocument doc;

    _config_check_valid();

    // Copy
    // doc["brightness"] = _config.brightness;
    // doc["orientation"] = _config.orientation;
    if (_display != nullptr)
    {
        doc["brightness"] = _display->getBrightness();
        doc["orientation"] = _display->getRotation();
    }
    else
    {
        doc["brightness"] = _config.brightness;
        doc["orientation"] = _config.orientation;
    }

    doc["beepOn"] = _config.beepOn;
    doc["highRefreshRate"] = _config.highRefreshRate;
    doc["reverseEncoder"] = _config.reverseEncoder;
    doc["localeCode"] = _config.localeCode;
    doc["currentOffset"] = _config.currentOffset;
    doc["wifiSsid"] = _config.wifiSsid;
    doc["wifiPassword"] = _config.wifiPassword;
    doc["startupImage"] = _config.startupImage;

    // Serialize
    std::string json_content;
    if (serializeJson(doc, json_content) == 0)
    {
        _disp_init();
        popFatalError("Serialize failed");
    }

    return json_content;
}

void HAL_VAMeter::_backup_config_file()
{
    spdlog::info("create config backup");

    // Config
    spdlog::info("open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path, "rb");
    if (config_file == NULL)
    {
        spdlog::warn("open config failed, return");
        return;
    }

    // Backup
    spdlog::info("open {}", _system_config_backup_path);
    FILE* config_backup_file = fopen(_system_config_backup_path, "wb");
    if (config_backup_file == NULL)
    {
        _disp_init();
        popFatalError("Open config backup failed");
    }

    // Copy and save
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), config_file)) > 0)
    {
        fwrite(buffer, 1, bytesRead, config_backup_file);
    }

    fclose(config_file);
    fclose(config_backup_file);
}

void HAL_VAMeter::saveSystemConfig()
{
    spdlog::info("save config to fs");

    _backup_config_file();

    std::string json_content = _create_config_json();

    // Open file
    spdlog::info("open {}", _system_config_path);
    FILE* config_file = fopen(_system_config_path, "wb");
    if (config_file == NULL)
    {
        _disp_init();
        popFatalError("Open config failed");
    }

    // Write and save
    fputs(json_content.c_str(), config_file);
    fclose(config_file);

    spdlog::info("config saved: {}", _system_config_path);
}

/* -------------------------------------------------------------------------- */
/*                                    Misc                                    */
/* -------------------------------------------------------------------------- */
std::vector<std::string> HAL_VAMeter::_ls(const std::string& path)
{
    spdlog::info("ls {}", path);

    std::vector<std::string> result;

    auto folder = opendir(path.c_str());

    // If no directory
    if (folder == NULL)
    {
        return result;
    }

    // Iterate all rec files
    struct dirent* entry;
    while ((entry = readdir(folder)) != NULL)
    {
        // pass
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file name
        result.push_back(entry->d_name);
    }

    // Close folder
    closedir(folder);

    return result;
}

/* -------------------------------------------------------------------------- */
/*                                  MSC mode                                  */
/* -------------------------------------------------------------------------- */
void HAL_VAMeter::startMscMode()
{
    spdlog::info("start msc mode");
    msc_mode_init();
}

void HAL_VAMeter::stopMscMode()
{
    spdlog::info("stop msc mode");
    msc_mode_deinit();
}

/* -------------------------------------------------------------------------- */
/*                                Factory reset                               */
/* -------------------------------------------------------------------------- */
void HAL_VAMeter::factoryReset(OnLogPageRenderCallback_t onLogPageRender)
{
    std::string string_buffer = "start factory reset..";
    onLogPageRender(string_buffer, true, true);
    spdlog::info(string_buffer);

    if (!fs_format())
        popFatalError("Factory format\nFailed");

    reboot();
}

/* -------------------------------------------------------------------------- */
/*                              Rec File related                              */
/* -------------------------------------------------------------------------- */
// Rec file name:
// REC-001.csv
const char* _rec_folder_path = "/spiflash/rec";
const char* _rec_file_name_prefix = "REC-";
const char* _rec_file_name_suffix = ".csv";

void HAL_VAMeter::_fs_get_new_rec_file_path(char* recFilePath, size_t bufferSize)
{
    auto folder = opendir(_rec_folder_path);

    // If no directory
    if (folder == NULL)
    {
        // Create folder
        spdlog::info("no floder {}, creating", _rec_folder_path);
        if (mkdir(_rec_folder_path, S_IRWXU) != 0)
        {
            popFatalError("Create rec\nFolder failed");
        }

        // Default file name
        snprintf(recFilePath, bufferSize, "%s/%s%03d%s", _rec_folder_path, _rec_file_name_prefix, 0, _rec_file_name_suffix);
        spdlog::info("get new rec file path: {}", recFilePath);
        return;
    }

    // Iterate all rec files
    int rec_file_max_id = -1;
    struct dirent* entry;
    while ((entry = readdir(folder)) != NULL)
    {
        // pass
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file id
        int file_id = atoi(entry->d_name + 4);

        // Update
        if (file_id > rec_file_max_id)
            rec_file_max_id = file_id;
    }

    // Close folder
    closedir(folder);

    if (rec_file_max_id < 0)
        rec_file_max_id = 0;

    // Get file path on top of max id
    snprintf(recFilePath,
             bufferSize,
             "%s/%s%03d%s",
             _rec_folder_path,
             _rec_file_name_prefix,
             rec_file_max_id + 1,
             _rec_file_name_suffix);

    spdlog::info("get new rec file path: {}", recFilePath);
}

std::string HAL_VAMeter::_fs_get_rec_file_path(const std::string& recordName)
{
    std::string path = _rec_folder_path;
    path += "/";
    path += recordName;
    return path;
}

std::vector<std::string> HAL_VAMeter::getVaRecordNameList() { return _ls(_rec_folder_path); }

std::string HAL_VAMeter::getLatestVaRecordName()
{
    std::string result;

    auto folder = opendir(_rec_folder_path);

    // If no directory
    if (folder == NULL)
    {
        spdlog::warn("no record file");
        return result;
    }

    // Iterate all rec files
    int rec_file_max_id = -1;
    struct dirent* entry;
    while ((entry = readdir(folder)) != NULL)
    {
        // pass
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file id
        int file_id = atoi(entry->d_name + 4);

        // Update
        if (file_id > rec_file_max_id)
        {
            rec_file_max_id = file_id;
            result = entry->d_name;
        }
    }

    // Close folder
    closedir(folder);

    spdlog::info("get latest va record name: {}", result);
    return result;
}

static constexpr uint16_t _max_reading_line = 10000 / 40;

VA_RECORDER::Record HAL_VAMeter::getVaRecord(const std::string& recordName)
{
    VA_RECORDER::Record result;

    // Get path
    std::string record_path = _rec_folder_path;
    record_path += "/";
    record_path += recordName;

    spdlog::info("try reading {}", record_path);

    // Try open
    FILE* record_file = fopen(record_path.c_str(), "r");
    if (record_file == NULL)
    {
        spdlog::warn("{} not exist", record_path);
        return result;
    }

    // Skip first two line
    fscanf(record_file, "%*[^\n]\n");
    fscanf(record_file, "%*[^\n]\n");

    // Read line by line
    uint16_t line_count = 0;
    while (1)
    {
        result.push_back(VA_RECORDER::RecordData_t());

        if (fscanf(record_file, "%f,%f\n", &result[line_count].voltage, &result[line_count].current) != 2)
            break;

        // spdlog::info("get {} {}", result[line_count].voltage, result[line_count].current);

        line_count++;
        if (line_count > _max_reading_line)
            break;
    }

    fclose(record_file);

    spdlog::info("done, read {} lines", line_count);
    return result;
}

bool HAL_VAMeter::deleteVaRecord(const std::string& recordName)
{
    // Get path
    std::string record_path = _rec_folder_path;
    record_path += "/";
    record_path += recordName;

    spdlog::info("rm {}", record_path);

    if (remove(record_path.c_str()) == 0)
    {
        spdlog::info("done");
        return true;
    }
    else
    {
        spdlog::error("rm failed");
        return false;
    }
}

static const char* _startup_image_folder_path = "/spiflash/startup_logo";

bool HAL_VAMeter::renderCustomStartupImage()
{
    // Check folder
    {
        auto folder = opendir(_startup_image_folder_path);

        // If no directory
        if (folder == NULL)
        {
            // Create folder
            spdlog::info("no floder {}, creating", _startup_image_folder_path);
            if (mkdir(_startup_image_folder_path, S_IRWXU) != 0)
                popFatalError("Create StartupLogo\nFolder failed");

            return false;
        }
        closedir(folder);
    }

    if (_config.startupImage == "")
        return false;

    std::string image_path = _startup_image_folder_path;
    image_path += "/";
    image_path += _config.startupImage;
    spdlog::info("try render {}", image_path);

    // _canvas->setFileStorage(FILE);
    return _canvas->drawJpgFile(image_path.c_str(), 0, 0, _canvas->width(), _canvas->height());
}

std::vector<std::string> HAL_VAMeter::getStartupImageList() { return _ls(_startup_image_folder_path); }
