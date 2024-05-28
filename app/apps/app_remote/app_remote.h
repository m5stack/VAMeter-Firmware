/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include <cstdint>
#include <mooncake.h>

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Remote
         *
         */
        class AppRemote : public APP_BASE
        {
        private:
            struct Data_t
            {
                std::uint32_t time_count = 0;
            };
            Data_t _data;

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppRemote_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override { return "Remote"; }
            void* getAppIcon() override;
            void* newApp() override { return new AppRemote; }
            void deleteApp(void* app) override { delete (AppRemote*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE
