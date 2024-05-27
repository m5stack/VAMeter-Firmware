/**
 * @file hal_upload.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-04-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <Arduino.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

static const char* _server_host = "ezdata2.m5stack.com";
static const uint16_t _server_port = 80;

struct UploadData_t
{
    FILE* record_file = NULL;
    long record_file_size = 0;
    uint8_t* record_read_chunk = nullptr;

    std::string string_buffer;
    std::string form_data_part_1;
    std::string form_data_part_2;

    ~UploadData_t()
    {
        // Collect garbage
        if (record_file != NULL)
            fclose(record_file);
        if (record_read_chunk != nullptr)
            delete[] record_read_chunk;
    }
};

bool HAL_VAMeter::uploadVaRecordViaEzData(const std::string& recordName, OnLogPageRenderCallback_t onLogPageRender)
{
    if (!connectWifi(onLogPageRender, false))
        return false;

    UploadData_t data;

    /* -------------------------------------------------------------------------- */
    /*                                 Record file                                */
    /* -------------------------------------------------------------------------- */
    onLogPageRender(" > open record file\n", true, true);
    {
        std::string record_path = _fs_get_rec_file_path(recordName);
        spdlog::info("try open {}", record_path);

        data.record_file = fopen(record_path.c_str(), "rb");
        if (data.record_file == NULL)
        {
            popWarning("Open record failed");
            return false;
        }

        // Get file size
        fseek(data.record_file, 0, SEEK_END);
        data.record_file_size = ftell(data.record_file);
        spdlog::info("get file size {}", data.record_file_size);

        // Seek back
        fseek(data.record_file, 0, SEEK_SET);
    }

    /* -------------------------------------------------------------------------- */
    /*                                  Form data                                 */
    /* -------------------------------------------------------------------------- */
    data.form_data_part_1 =
        spdlog::fmt_lib::format("\r\nContent-Type: multipart/form-data; "
                                "boundary=----VAMeter\r\n\r\n------VAMeter\r\nContent-Disposition: "
                                "form-data; name=\"dataType\"\r\n\r\nfile\r\n------VAMeter\r\nContent-Disposition: "
                                "form-data; name=\"name\"\r\n\r\nrec\r\n------VAMeter\r\nContent-Disposition: form-data; "
                                "name=\"value\"\r\n\r\n{}\r\n------VAMeter\r\nContent-Disposition: form-data; name=\"file\"; "
                                "filename=\"{}\"\r\nContent-Type: text/csv\r\n",
                                recordName,
                                recordName);

    data.form_data_part_2 = spdlog::fmt_lib::format("\r\n------VAMeter\r\nContent-Disposition: form-data; "
                                                    "name=\"deviceType\"\r\n\r\nstamps3\r\n------VAMeter--\r\n");

    /* -------------------------------------------------------------------------- */
    /*                                   Request                                  */
    /* -------------------------------------------------------------------------- */
    data.string_buffer = spdlog::fmt_lib::format(" > connecting server:\n > {}:{}\n", _server_host, _server_port);
    onLogPageRender(data.string_buffer, true, false);

    // Connect server
    WiFiClient client;
    if (!client.connect(_server_host, _server_port))
    {
        popWarning("Connect server\nFailed");
        return false;
    }

    int content_length = data.form_data_part_1.size() + data.record_file_size + data.form_data_part_2.size();
    spdlog::info("get content length {}", content_length);

    onLogPageRender(" > send request..\n", true, false);

    /* --------------------------------- Header --------------------------------- */
    spdlog::info("send header");

    data.string_buffer = spdlog::fmt_lib::format("POST /api/v2/{}/uploadMacFile HTTP/1.1", _get_mac());
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    data.string_buffer = spdlog::fmt_lib::format("Host: {}:{}", _server_host, _server_port);
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    data.string_buffer = spdlog::fmt_lib::format("Content-Length: {}", content_length);
    client.println(data.string_buffer.c_str());
    // printf("%s\r\n", data.string_buffer.c_str());

    client.println("Content-Type: multipart/form-data; boundary=----VAMeter");
    // printf("Content-Type: multipart/form-data; boundary=----VAMeter\r\n");

    /* -------------------------------- Form data ------------------------------- */
    /* --------------------------------- Part 1 --------------------------------- */
    // spdlog::info("send part 1");
    client.println(data.form_data_part_1.c_str());
    // printf("%s\r\n", data.form_data_part_1.c_str());
    std::string().swap(data.form_data_part_1);

    /* -------------------------------- Part file ------------------------------- */
    // spdlog::info("send file");
    data.record_read_chunk = new uint8_t[2048];
    size_t bytes_read = 0;
    while ((bytes_read = fread(data.record_read_chunk, 1, 2048, data.record_file)) > 0)
    {
        feedTheDog();
        spdlog::info("read and send {} ..", bytes_read);
        client.write(data.record_read_chunk, bytes_read);

        data.string_buffer = spdlog::fmt_lib::format(" > read and send {} ..\n", bytes_read);
        onLogPageRender(data.string_buffer, true, false);
    }
    delete[] data.record_read_chunk;
    data.record_read_chunk = nullptr;
    // printf("(data)");

    /* --------------------------------- Part 2 --------------------------------- */
    // spdlog::info("send part 2");
    client.println(data.form_data_part_2.c_str());
    // printf("%s\r\n", data.form_data_part_2.c_str());

    /* -------------------------------------------------------------------------- */
    /*                                  Response                                  */
    /* -------------------------------------------------------------------------- */
    onLogPageRender(" > wait response..\n", true, false);

    // Expose response handler
    class MyHttpClient : public HTTPClient
    {
    public:
        int myHandleHeaderResponse() { return HTTPClient::handleHeaderResponse(); }
    };
    MyHttpClient http;
    http.begin(client, "");

    // Wait and parse response
    spdlog::info("wait response");
    feedTheDog();
    auto status_code = http.myHandleHeaderResponse();
    data.string_buffer = http.getString().c_str();
    spdlog::info("get code: {}\npayload: {}", status_code, data.string_buffer);

    if (status_code != 200)
    {
        data.string_buffer = spdlog::fmt_lib::format("Bad response\nCode: {}", status_code);
        popWarning(data.string_buffer);
        return false;
    }

    client.stop();
    return true;
}

std::string HAL_VAMeter::getVaRecordEzDataUrl()
{
    std::string ret = "https://ezdata-vameter.m5stack.com/";
    ret += _get_mac();
    return ret;
}
