/**
 * @file hal_va_recorder.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <string>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <dirent.h>
#include <esp_task_wdt.h>

/* -------------------------------------------------------------------------- */
/*           A class to handle communication between daemon and hal           */
/* -------------------------------------------------------------------------- */
enum RunningState_t
{
    daemon_state_preparing = 0,
    daemon_state_waiting,
    daemon_state_recording,
    daemon_state_wraping_up,
    daemon_state_finished,
};

struct RecorderDaemonStatus_t
{
public:
    struct Data_t
    {
        char savePath[30] = {0};
        RunningState_t currentState = daemon_state_preparing;
        VA_RECORDER::TriggerBase* trigger = nullptr;
        bool goToHell = false;
        float current_offset = 0.0f;
    };

private:
    Data_t _data;
    SemaphoreHandle_t _mutex;

public:
    RecorderDaemonStatus_t() { _mutex = xSemaphoreCreateMutex(); }
    ~RecorderDaemonStatus_t() { vSemaphoreDelete(_mutex); }
    Data_t& Borrow()
    {
        xSemaphoreTake(_mutex, portMAX_DELAY);
        return _data;
    }
    void Return() { xSemaphoreGive(_mutex); }
    Data_t& GetRaw() { return _data; }
};

/* -------------------------------------------------------------------------- */
/*                                    Errno                                   */
/* -------------------------------------------------------------------------- */
static const char* _errno_open_temp_dir_failed = "Open temp dir\nFailed";
static const char* _errno_clear_temp_dir_failed = "Clear temp dir\nFailed";
static const char* _errno_remove_temp_dir_failed = "Clear remove dir\nFailed";
static const char* _errno_create_temp_dir_failed = "Clear create dir\nFailed";
static const char* _errno_open_chunk_failed = "Open chunk\nFailed";
static const char* _errno_close_chunk_failed = "Close chunk\nFailed";
static const char* _errno_open_rec_file_failed = "Open rec file\nFailed";
static const char* _errno_close_rec_file_failed = "Close rec file\nFailed";
static const char* _errno_malloc_failed = "Malloc failed";

/* -------------------------------------------------------------------------- */
/*                            Recorder deamon task                            */
/* -------------------------------------------------------------------------- */
POWER_MONITOR::PMData_t* _borrow_pm_data_daemon();
void _return_pm_data_daemon();

// 选👴! OS 的神
void _remove_temp_dir(const char* tempDirPath)
{
    spdlog::info("try remove {}", tempDirPath);

    // Clear dir
    DIR* dir = opendir(tempDirPath);
    if (dir == NULL)
        HAL::PopFatalError(_errno_open_temp_dir_failed);

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", tempDirPath, entry->d_name);
            if (remove(path) != 0)
                HAL::PopFatalError(_errno_clear_temp_dir_failed);
        }

        esp_task_wdt_reset();
    }

    closedir(dir);
    spdlog::info("dir clear");

    // Remove dir
    if (rmdir(tempDirPath) == -1)
        HAL::PopFatalError(_errno_remove_temp_dir_failed);

    spdlog::info("remove done");
}

static void _va_recorder_daemon(void* param)
{
    esp_task_wdt_add(NULL);
    bool is_going_to_hell = false;

    const char* temp_folder_path = "/spiflash/temp";
    FILE* rec_final_file = NULL;
    FILE* rec_chunk_file = NULL;
    uint16_t rec_chunk_file_count = 0;
    char rec_chunk_file_path_buffer[30] = {0};

    char* chunk_read_buffer = NULL;
    size_t chunk_read_buffer_size = 0;
    uint32_t rec_chunk_save_interval = 5000;
    uint32_t millis = 0;

    // uint32_t sample_time_count = 0;
    uint32_t chunk_save_time_count = 0;
    uint32_t recording_time_count = 0;

    // Get status
    RecorderDaemonStatus_t* daemon_status = (RecorderDaemonStatus_t*)param;

    // Get trigger and buffer
    VA_RECORDER::TriggerBase* trigger = daemon_status->Borrow().trigger;
    VA_RECORDER::PreTriggerBuffer* pre_trigger_buffer = nullptr;

    if (daemon_status->GetRaw().trigger->preTriggerBufferSize() != 0)
    {
        spdlog::info("create pretrigger buffer in size: {}", daemon_status->GetRaw().trigger->preTriggerBufferSize());
        pre_trigger_buffer = new VA_RECORDER::PreTriggerBuffer;
        pre_trigger_buffer->reSize(daemon_status->GetRaw().trigger->preTriggerBufferSize());
    }
    else
        spdlog::info("no pretrigger buffer");

    /* -------------------------------------------------------------------------- */
    /*                               State preparing                              */
    /* -------------------------------------------------------------------------- */
    // Create temp folder
    if (access(temp_folder_path, F_OK) == 0)
    {
        _remove_temp_dir(temp_folder_path);
    }
    if (mkdir(temp_folder_path, S_IRWXU) != 0)
    {
        daemon_status->Return();
        HAL::PopFatalError(_errno_create_temp_dir_failed);
    }

    daemon_status->Return();

    /* -------------------------------------------------------------------------- */
    /*                                State waiting                               */
    /* -------------------------------------------------------------------------- */
    daemon_status->Borrow().currentState = daemon_state_waiting;
    daemon_status->Return();

    // Create chunk file
    rec_chunk_file_count++;
    snprintf(
        rec_chunk_file_path_buffer, sizeof(rec_chunk_file_path_buffer), "%s/%d.csv", temp_folder_path, rec_chunk_file_count);

    spdlog::info("open {}", rec_chunk_file_path_buffer);
    rec_chunk_file = fopen(rec_chunk_file_path_buffer, "w");
    if (rec_chunk_file == NULL)
        HAL::PopFatalError("Open chunk failed");

    while (1)
    {
        // Push pretrigger buffer
        if (pre_trigger_buffer != nullptr)
        {
            auto pm_data = _borrow_pm_data_daemon();
            pre_trigger_buffer->put(VA_RECORDER::RecordData_t(pm_data->busVoltage, pm_data->shuntCurrent));
            _return_pm_data_daemon();
        }

        // If triggered
        if (trigger->onCheck(pre_trigger_buffer))
            break;

        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(trigger->getSampleInterval()));

        // Check kill signal
        is_going_to_hell = daemon_status->Borrow().goToHell;
        daemon_status->Return();
        if (is_going_to_hell)
            goto HELL;
    }

    /* -------------------------------------------------------------------------- */
    /*                               State recording                              */
    /* -------------------------------------------------------------------------- */
    daemon_status->Borrow().currentState = daemon_state_recording;
    daemon_status->Return();

    HAL::ResetPowerMonitorData();

    millis = esp_timer_get_time() / 1000;
    recording_time_count = millis;
    chunk_save_time_count = millis;
    while (1)
    {
        // Normal writing
        auto pm_data = _borrow_pm_data_daemon();
        fprintf(rec_chunk_file, "%.4f,%.7f\n", pm_data->busVoltage, pm_data->shuntCurrent);
        _return_pm_data_daemon();

        // Time out check
        millis = esp_timer_get_time() / 1000;
        if (millis - recording_time_count > trigger->getRecordTime())
        {
            break;
        }

        // Chunk saving
        millis = esp_timer_get_time() / 1000;
        if (millis - chunk_save_time_count > rec_chunk_save_interval)
        {
            spdlog::info("chunk saving..");

            // Close chunk file
            if (fclose(rec_chunk_file) == EOF)
                HAL::PopFatalError("Close chunk failed");
            rec_chunk_file = NULL;

            rec_chunk_file_count++;
            snprintf(rec_chunk_file_path_buffer,
                     sizeof(rec_chunk_file_path_buffer),
                     "%s/%d.csv",
                     temp_folder_path,
                     rec_chunk_file_count);
            spdlog::info("open {}", rec_chunk_file_path_buffer);

            // Create new chunk
            rec_chunk_file = fopen(rec_chunk_file_path_buffer, "w");
            if (rec_chunk_file == NULL)
                HAL::PopFatalError("Open chunk failed");

            millis = esp_timer_get_time() / 1000;
            chunk_save_time_count = millis;
        }

        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(trigger->getSampleInterval()));

        // Check kill signal
        is_going_to_hell = daemon_status->Borrow().goToHell;
        daemon_status->Return();
        if (is_going_to_hell)
            goto HELL;
    }

    // Close chunk file
    if (fclose(rec_chunk_file) == EOF)
        HAL::PopFatalError("Close chunk failed");
    rec_chunk_file = NULL;

    /* -------------------------------------------------------------------------- */
    /*                                State wrap up                               */
    /* -------------------------------------------------------------------------- */
    daemon_status->Borrow().currentState = daemon_state_wraping_up;
    daemon_status->Return();

    spdlog::info("start wraping, chunk num: {}", rec_chunk_file_count);
    spdlog::info("open {}", daemon_status->Borrow().savePath);

    /* --------------------------- Write file starter --------------------------- */
    rec_final_file = fopen(daemon_status->GetRaw().savePath, "w");
    daemon_status->Return();
    if (rec_final_file == NULL)
        HAL::PopFatalError(_errno_open_rec_file_failed);

    fprintf(rec_final_file, "voltage,current,time,capacity,energy\n");
    {
        auto pm_data = _borrow_pm_data_daemon();
        fprintf(rec_final_file, ",,%ld,%.7f,%.7f\n", pm_data->time, pm_data->capacity, pm_data->energy);
        _return_pm_data_daemon();
    }

    if (fclose(rec_final_file) == EOF)
        HAL::PopFatalError(_errno_close_rec_file_failed);
    rec_final_file = NULL;

    /* ------------------------- Write pretrigger buffer ------------------------ */
    // If has pretrigger buffer
    if (pre_trigger_buffer != nullptr)
    {
        spdlog::info("write pretrigger buffer");

        // Open final file
        rec_final_file = fopen(daemon_status->Borrow().savePath, "a");
        daemon_status->Return();
        if (rec_final_file == NULL)
            HAL::PopFatalError(_errno_open_rec_file_failed);

        // Write pretrigger buffer into file, and free
        pre_trigger_buffer->peekAll([rec_final_file](const VA_RECORDER::RecordData_t& recData) {
            fprintf(rec_final_file, "%.4f,%.7f\n", recData.voltage, recData.current);
        });
        delete pre_trigger_buffer;
        pre_trigger_buffer = nullptr;

        // Save
        if (fclose(rec_final_file) == EOF)
            HAL::PopFatalError(_errno_close_rec_file_failed);
        rec_final_file = NULL;
    }

    /* ------------------------------ Merge chunks ------------------------------ */
    // Write all chunks into final file
    for (int i = 1; i < rec_chunk_file_count + 1; i++)
    {
        snprintf(rec_chunk_file_path_buffer, sizeof(rec_chunk_file_path_buffer), "%s/%d.csv", temp_folder_path, i);
        spdlog::info("try append {}", rec_chunk_file_path_buffer);

        // Open chunk file
        rec_chunk_file = fopen(rec_chunk_file_path_buffer, "rb");
        if (rec_chunk_file == NULL)
            HAL::PopFatalError(_errno_open_chunk_failed);

        // Get size and allocate
        fseek(rec_chunk_file, 0, SEEK_END);
        chunk_read_buffer_size = ftell(rec_chunk_file);
        fseek(rec_chunk_file, 0, SEEK_SET);

        chunk_read_buffer = (char*)malloc(chunk_read_buffer_size * sizeof(char));
        if (chunk_read_buffer == NULL)
            HAL::PopFatalError(_errno_malloc_failed);

        // Copy buffer and close chunk
        fread(chunk_read_buffer, sizeof(char), chunk_read_buffer_size, rec_chunk_file);
        if (fclose(rec_chunk_file) == EOF)
            HAL::PopFatalError(_errno_close_chunk_failed);
        rec_chunk_file = NULL;

        // Open final file
        rec_final_file = fopen(daemon_status->Borrow().savePath, "a");
        daemon_status->Return();
        if (rec_final_file == NULL)
            HAL::PopFatalError(_errno_open_rec_file_failed);

        // Append buffer to the end
        fwrite(chunk_read_buffer, sizeof(char), chunk_read_buffer_size, rec_final_file);

        // Save and free
        if (fclose(rec_final_file) == EOF)
            HAL::PopFatalError(_errno_close_rec_file_failed);
        rec_final_file = NULL;

        free(chunk_read_buffer);
        chunk_read_buffer = NULL;

        esp_task_wdt_reset();
    }

    /* ------------------------------- Remove temp ------------------------------ */
    _remove_temp_dir(temp_folder_path);

    spdlog::info("done, saved at {}", daemon_status->Borrow().savePath);
    daemon_status->Return();

/* -------------------------------------------------------------------------- */
/*                                   Goodbye                                  */
/* -------------------------------------------------------------------------- */
HELL:
    // Check resource
    if (rec_final_file != NULL)
    {
        if (fclose(rec_final_file) == EOF)
            HAL::PopFatalError(_errno_close_rec_file_failed);
    }

    if (rec_chunk_file != NULL)
    {
        if (fclose(rec_chunk_file) == EOF)
            HAL::PopFatalError(_errno_close_rec_file_failed);
    }

    if (chunk_read_buffer != NULL)
        free(chunk_read_buffer);

    if (pre_trigger_buffer != nullptr)
        delete pre_trigger_buffer;

    daemon_status->Borrow().currentState = daemon_state_finished;
    daemon_status->Return();
    esp_task_wdt_delete(NULL);
    vTaskDelete(NULL);
}

/* -------------------------------------------------------------------------- */
/*                                    Apis                                    */
/* -------------------------------------------------------------------------- */
static RecorderDaemonStatus_t* _recorder_daemon_status = nullptr;

bool HAL_VAMeter::creatVaRecorder(VA_RECORDER::TriggerBase* trigger)
{
    if (_recorder_daemon_status != nullptr)
    {
        spdlog::error("recoder already exist");
        return false;
    }
    spdlog::info("create recoder: rec time: {} ms, interval: {} ms", trigger->getRecordTime(), trigger->getSampleInterval());

    // Create status
    _recorder_daemon_status = new RecorderDaemonStatus_t;
    _fs_get_new_rec_file_path(_recorder_daemon_status->GetRaw().savePath, sizeof(_recorder_daemon_status->GetRaw().savePath));
    _recorder_daemon_status->GetRaw().trigger = trigger;
    _recorder_daemon_status->GetRaw().current_offset = _config.currentOffset;

    // Create daemon
    xTaskCreate(_va_recorder_daemon, "rec", 4000, (void*)_recorder_daemon_status, 15, NULL);
    return true;
}

bool HAL_VAMeter::isVaRecorderExist()
{
    if (_recorder_daemon_status == nullptr)
    {
        spdlog::error("recoder not exist");
        return false;
    }

    bool is_running = _recorder_daemon_status->Borrow().currentState != daemon_state_finished;
    _recorder_daemon_status->Return();
    return is_running;
}

bool HAL_VAMeter::isVaRecorderRecording()
{
    if (_recorder_daemon_status == nullptr)
    {
        spdlog::error("recoder not exist");
        return false;
    }

    bool is_recording = _recorder_daemon_status->Borrow().currentState == daemon_state_recording;
    _recorder_daemon_status->Return();
    return is_recording;
}

bool HAL_VAMeter::isVaRecorderSaving()
{
    if (_recorder_daemon_status == nullptr)
    {
        spdlog::error("recoder not exist");
        return false;
    }

    bool is_saving = _recorder_daemon_status->Borrow().currentState == daemon_state_wraping_up;
    _recorder_daemon_status->Return();
    return is_saving;
}

bool HAL_VAMeter::destroyVaRecorder()
{
    if (_recorder_daemon_status == nullptr)
    {
        spdlog::error("recoder not exist");
        return false;
    }

    // Destroy daemon
    if (isVaRecorderExist())
    {
        spdlog::info("recorder runing, kill signal send");
        _recorder_daemon_status->Borrow().goToHell = true;
        _recorder_daemon_status->Return();
        while (1)
        {
            feedTheDog();
            vTaskDelay(pdMS_TO_TICKS(50));

            if (!isVaRecorderExist())
                break;
        }
    }

    // Destroy recorde
    // delete _recorder_daemon_status->GetRaw().trigger;
    delete _recorder_daemon_status;
    _recorder_daemon_status = nullptr;
    spdlog::info("recoder destroyed");
    return true;
}
