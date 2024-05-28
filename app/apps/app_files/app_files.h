/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <cstdint>
#include <mooncake.h>
#include "../utils/system/system.h"

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Files
         *
         */
        class AppFiles : public APP_BASE
        {
        private:
            struct Data_t
            {
                bool is_just_delete_file = false;
            };
            Data_t _data;
            void _on_page_root();
            void _on_page_record_files();
            void _on_record_file_open(const std::string& fileName);
            void _on_page_msc_mode();

        public:
            static void _on_page_upload_success(const std::string& recordName);

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppFiles_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override;
            void* getAppIcon() override;
            void* newApp() override { return new AppFiles; }
            void deleteApp(void* app) override { delete (AppFiles*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE
