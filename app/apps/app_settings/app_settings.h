/**
 * @file app_settings.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <cstdint>
#include <mooncake.h>
#include "../utils/system/system.h"

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Settings
         *
         */
        class AppSettings : public APP_BASE
        {
        private:
            struct Data_t
            {
                SYSTEM::UI::SelectMenuPage::Theme_t select_page_theme;
                uint8_t mystery_count = 0;
            };
            Data_t _data;
            void _on_page_root();
            void _on_page_display();
            void _on_page_orientation();
            void _on_page_refresh_rate();
            void _on_page_buzzer();
            void _on_page_encoder();
            void _on_page_language();
            void _on_page_startup_image();
            void _on_page_calibration();
            void _on_page_network();
            void _on_page_ota_upgrade();
            void _on_page_about();
            void _on_page_base_test();
            void _on_factory_reset();

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppSettings_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override;
            void* getAppIcon() override;
            void* newApp() override { return new AppSettings; }
            void deleteApp(void* app) override { delete (AppSettings*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE
