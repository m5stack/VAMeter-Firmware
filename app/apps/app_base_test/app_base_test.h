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
         * @brief Base_test
         *
         */
        class AppBase_test : public APP_BASE
        {
        private:
            struct Data_t
            {
                uint32_t time_count = 0;
            };
            Data_t _data;

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppBase_test_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override { return "BaseTest"; }
            void* getAppIcon() override;
            void* newApp() override { return new AppBase_test; }
            void deleteApp(void* app) override { delete (AppBase_test*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE
