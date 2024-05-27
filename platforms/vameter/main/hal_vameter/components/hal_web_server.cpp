/**
 * @file hal_web_server.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include "../../../app/assets/assets.h"
#include <mooncake.h>
#include <Arduino.h>
#include <PsychicHttp.h>
#include <FS.h>
#include <vfs_api.h>

// class VFS_t : public FS
// {
// public:
//     VFS_t() : FS(FSImplPtr(new VFSImpl())) {}
// };
// static VFS_t VFS;

static PsychicHttpServer* _web_server = nullptr;
static PsychicWebSocketHandler* _ws_pm_data = nullptr;

/* -------------------------------------------------------------------------- */
/*                                    Pages                                   */
/* -------------------------------------------------------------------------- */
class MyChunkResponse : public PsychicResponse
{
private:
    uint8_t* _src = nullptr;
    size_t _size = 0;

public:
    MyChunkResponse(PsychicRequest* request, const String& contentType, uint8_t* src, size_t size)
        : PsychicResponse(request), _src(src), _size(size)
    {
        setContentType(contentType.c_str());
    }

    ~MyChunkResponse() {}

    esp_err_t send()
    {
        esp_err_t err = ESP_OK;

        // just send small files directly
        if (_size < FILE_CHUNK_SIZE)
        {
            this->setContent(_src, _size);
            err = PsychicResponse::send();
        }
        else
        {
            /* Retrieve the pointer to scratch buffer for temporary storage */
            char* chunk = (char*)malloc(FILE_CHUNK_SIZE);
            if (chunk == NULL)
            {
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(this->_request->request(), HTTPD_500_INTERNAL_SERVER_ERROR, "Unable to allocate memory.");
                return ESP_FAIL;
            }

            this->sendHeaders();

            size_t chunk_index = 0;
            size_t chunk_size = FILE_CHUNK_SIZE;
            while (1)
            {
                // Send chunk
                err = this->sendChunk(_src + chunk_index, chunk_size);
                if (err != ESP_OK)
                    break;

                chunk_index += chunk_size;
                if (chunk_index + FILE_CHUNK_SIZE > _size)
                    chunk_size = _size - chunk_index;
                else
                    chunk_size = FILE_CHUNK_SIZE;

                if (chunk_size == 0)
                    break;
            }

            // keep track of our memory
            free(chunk);

            if (err == ESP_OK)
            {
                ESP_LOGI(PH_TAG, "File sending complete");
                this->finishChunking();
            }
        }

        return err;
    }
};

void HAL_VAMeter::_web_server_page_loading()
{
    _web_server->on("/", [&](PsychicRequest* request) { return request->redirect("/syscfg"); });

    _web_server->on("/syscfg", [&](PsychicRequest* request) {
        MyChunkResponse response(
            request, "text/html", (uint8_t*)AssetPool::GetWebPage().syscfg, sizeof(AssetPool::GetWebPage().syscfg));
        return response.send();
    });

    _web_server->on("/favicon.ico", [&](PsychicRequest* request) {
        MyChunkResponse response(
            request, "image/x-icon", (uint8_t*)AssetPool::GetWebPage().favicon, sizeof(AssetPool::GetWebPage().favicon));
        return response.send();
    });
}

/* -------------------------------------------------------------------------- */
/*                                 Normal apis                                */
/* -------------------------------------------------------------------------- */
void HAL_VAMeter::_web_server_api_loading()
{
    _web_server->on("/api/get_net_info", [&](PsychicRequest* request) {
        std::string string_buffer;
        {
            JsonDocument doc;
            doc["mac"] = _get_mac();
            doc["ip"] = _get_ip();

            serializeJson(doc, string_buffer);
        }
        return request->reply(string_buffer.c_str());
    });

    _web_server->on("/api/set_syscfg", HTTP_POST, [&](PsychicRequest* request) {
        // spdlog::info("get json:\n{}", request->body().c_str());
        spdlog::info("handle set config");

        // Parse
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request->body().c_str());
        if (error != DeserializationError::Ok)
        {
            spdlog::error("json parse failed");
            spdlog::error("get:\n{}", request->body().c_str());
            return request->reply(500, "application/json", "{\"msg\":\"json parse failed\"}");
        }

        // Copy
        std::string string_buffer;

        string_buffer = doc["wifiSsid"].as<std::string>();
        if (string_buffer != "null")
            _config.wifiSsid = string_buffer;

        string_buffer = doc["wifiPassword"].as<std::string>();
        if (string_buffer != "null")
            _config.wifiPassword = string_buffer;

        // ...

        saveSystemConfig();

        return request->reply(200, "application/json", "{\"msg\":\"ok\"}");
    });

    _web_server->on("/api/get_wifi_list", [&](PsychicRequest* request) {
        std::string string_buffer;

        auto wifi_list = _get_wifi_list();

        // Encode
        JsonDocument doc;
        for (int i = 0; i < wifi_list.size(); i++)
        {
            doc["wifiList"][i] = wifi_list[i];
        }
        serializeJson(doc, string_buffer);

        return request->reply(string_buffer.c_str());
    });

    _web_server->on("/api/get_syscfg", [&](PsychicRequest* request) {
        std::string string_buffer = _create_config_json();
        return request->reply(string_buffer.c_str());
    });
}

/* -------------------------------------------------------------------------- */
/*                                 Web socket                                 */
/* -------------------------------------------------------------------------- */
POWER_MONITOR::PMData_t* _borrow_pm_data_daemon();
void _return_pm_data_daemon();

static void _ws_pm_data_daemon(void* param)
{
    char* string_buffer = new char[35];
    POWER_MONITOR::PMData_t* pm_data = _borrow_pm_data_daemon();
    _return_pm_data_daemon();

    vTaskDelay(pdMS_TO_TICKS(2000));
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));

        _borrow_pm_data_daemon();
        snprintf(string_buffer, 35, "{\"v\":%.4f,\"a\":%.7f}", pm_data->busVoltage, pm_data->shuntCurrent);
        _return_pm_data_daemon();

        // _ws_pm_data->sendAll("{\"v\":0.0001,\"a\":0.0000001}");
        _ws_pm_data->sendAll(string_buffer);
    }

    delete[] string_buffer;
    vTaskDelete(NULL);
}

void HAL_VAMeter::_web_server_ws_api_loading()
{
    _ws_pm_data = new PsychicWebSocketHandler;
    _web_server->on("/api/ws/pm_data", _ws_pm_data);

    // Callbacks
    _ws_pm_data->onOpen([](PsychicWebSocketClient* client) {
        spdlog::info("[socket] connection #{} connected from {}", client->socket(), client->remoteIP().toString().c_str());
        client->sendMessage("Hello!");
    });

    _ws_pm_data->onFrame([](PsychicWebSocketRequest* request, httpd_ws_frame* frame) {
        spdlog::info("[socket] #{} sent: {}", request->client()->socket(), (char*)frame->payload);
        return request->reply(frame);
    });

    _ws_pm_data->onClose([](PsychicWebSocketClient* client) {
        spdlog::info("[socket] connection #{} closed from {}", client->socket(), client->remoteIP().toString().c_str());
    });

    // Daemon
    xTaskCreate(_ws_pm_data_daemon, "ws", 4000, NULL, 5, NULL);
}

/* -------------------------------------------------------------------------- */
/*                                 Web server                                 */
/* -------------------------------------------------------------------------- */
bool HAL_VAMeter::startWebServer(OnLogPageRenderCallback_t onLogPageRender, bool autoWifiMode)
{
    // if (!connectWifi(onLogPageRender, false))
    //     return false;

    // Auto wifi mode
    bool go_sta_mode = autoWifiMode;
HELL:
    if (go_sta_mode)
    {
        // Check valid
        if (_config.wifiSsid.empty())
            go_sta_mode = false;
        if (_config.wifiPassword.empty())
            go_sta_mode = false;

        // Try connect
        if (go_sta_mode)
            go_sta_mode = connectWifi(onLogPageRender, false);

        // If not
        if (!go_sta_mode)
            goto HELL;
    }
    else
    {
        onLogPageRender("start ap mode", true, true);
        _start_ap_mode();
    }

    onLogPageRender("start web server", true, true);
    // assert(_web_server == nullptr);
    if (_web_server == nullptr)
    {
        _web_server = new PsychicHttpServer;
        _web_server->listen(80);

        _web_server_page_loading();
        _web_server_api_loading();
        _web_server_ws_api_loading();
    }

    spdlog::info("web server started");

    return true;
}

bool HAL_VAMeter::stopWebServer()
{
    spdlog::info("stop web server");

    // Kill server
    // feedTheDog();
    // _web_server->stop();
    // delay(200);
    // feedTheDog();
    // delete _web_server;
    // delay(200);
    // _web_server = nullptr;

    // Stop ap
    _stop_ap_mode();

    return true;
}

std::string HAL_VAMeter::getSystemConfigUrl()
{
    std::string ret = "http://";
    ret += _get_ip();
    ret += "/syscfg";
    return ret;
}
