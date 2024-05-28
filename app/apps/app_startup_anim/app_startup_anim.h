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
         * @brief Template
         *
         */
        class AppStartupAnim : public APP_BASE
        {
        private:
        public:
            void onResume() override;
            void onDestroy() override;

            static void PopUpGuideMap(bool force = false);
        };

        class AppStartupAnim_Packer : public APP_PACKER_BASE
        {
            const char * getAppName() override { return "StartupAnim"; }
            void* newApp() override { return new AppStartupAnim; }
            void deleteApp(void* app) override { delete (AppStartupAnim*)app; }
        };
    } // namespace APPS
} // namespace MOONCAKE
