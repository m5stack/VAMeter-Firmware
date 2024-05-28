/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "../hal_vameter.h"
#include "../hal_config.h"
#include <mooncake.h>
#include <driver/gpio.h>

class LGFX_VAMeter : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

public:
    LGFX_VAMeter(bool highRefreshRate = true)
    {
        {
            auto cfg = _bus_instance.config();

            cfg.pin_mosi = HAL_PIN_LCD_MOSI;
            cfg.pin_miso = HAL_PIN_LCD_MISO;
            cfg.pin_sclk = HAL_PIN_LCD_SCLK;
            cfg.pin_dc = HAL_PIN_LCD_DC;

            if (highRefreshRate)
                cfg.freq_write = 80000000;
            else
                cfg.freq_write = 40000000;

            cfg.spi_3wire = false;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();

            cfg.invert = true;
            cfg.pin_cs = HAL_PIN_LCD_CS;
            cfg.pin_rst = HAL_PIN_LCD_RST;
            cfg.pin_busy = HAL_PIN_LCD_BUSY;
            cfg.panel_width = 240;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            // cfg.bus_shared   = true;

            _panel_instance.config(cfg);
        }
        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = HAL_PIN_LCD_BL;
            cfg.invert = false;
            // cfg.freq = 44100;
            // cfg.freq = 200;
            cfg.freq = 500;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};

void HAL_VAMeter::_disp_init()
{
    spdlog::info("display init");

    if (_display != nullptr)
    {
        spdlog::info("already inited");
        return;
    }

    // Refresh rate
    // _config.highRefreshRate = false;
    // _config.highRefreshRate = true;
    _display = new LGFX_VAMeter(_config.highRefreshRate);
    _display->init();

    // Brightness
    // Let boot anim to do the job
    _display->setBrightness(0);

    // // Enable DMA
    // _display->startWrite();

    if (_config.highRefreshRate)
    {
        gpio_set_drive_capability((gpio_num_t)HAL_PIN_LCD_CS, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability((gpio_num_t)HAL_PIN_LCD_DC, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability((gpio_num_t)HAL_PIN_LCD_MOSI, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability((gpio_num_t)HAL_PIN_LCD_SCLK, GPIO_DRIVE_CAP_3);
    }

    // Orientation
    _display->setRotation(_config.orientation);

    _canvas = new LGFX_SpriteFx(_display);
    _canvas->createSprite(_display->width(), _display->height());

    /* -------------------------------------------------------------------------- */
    /*                                    Test                                    */
    /* -------------------------------------------------------------------------- */
    // int bbb = 0;
    // _display->fillScreen(TFT_WHITE);
    // for (int j = 0; j < 3; j++)
    // {
    //     bbb = 0;
    //     for (int i = 0; i < 255; i++)
    //     {
    //         _display->setBrightness(bbb);
    //         bbb++;
    //         // spdlog::info("{}", bbb);
    //         delay(10);
    //     }
    // }

    // _display->setBrightness(255);
    // // while (1)
    // for (int i = 0; i < 1; i++)
    // {
    //     _canvas->fillScreen(TFT_RED);
    //     _canvas->pushSprite(0, 0);
    //     spdlog::info("r");
    //     delay(1000);
    //     _canvas->fillScreen(TFT_GREEN);
    //     _canvas->pushSprite(0, 0);
    //     spdlog::info("g");
    //     delay(1000);
    //     _canvas->fillScreen(TFT_BLUE);
    //     _canvas->pushSprite(0, 0);
    //     spdlog::info("b");
    //     delay(1000);
    // }

    // _canvas->setBaseColor(TFT_WHITE);
    // _canvas->setTextScroll(true);
    // _canvas->setTextSize(3);
    // _canvas->setTextColor(TFT_BLACK);
    // long aaaa = 0;
    // while (1)
    // {
    //     aaaa++;
    //     // _canvas->fillScreen(TFT_WHITE);
    //     _canvas->print(aaaa);
    //     _canvas->pushSprite(0, 0);
    // }
}
