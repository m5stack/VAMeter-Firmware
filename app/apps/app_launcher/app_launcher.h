/**
 * @file app_launcher.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-01-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "view/view.h"
#include <mooncake.h>

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Launcher
         *
         */
        class AppLauncher : public APP_BASE
        {
        private:
            struct Data_t
            {
                VIEW::LauncherView* launcher_view = nullptr;
            };
            Data_t _data;
            void _play_boot_anim();
            void _create_launcher_view();
            void _update_launcher_view();
            void _destroy_launcher_view();

        public:
            void onCreate() override;
            void onResume() override;
            void onRunning() override;
            void onRunningBG() override;
            void onDestroy() override;
            void onPause() override;
        };

        class AppLauncher_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override { return "Launcher"; }
            void* newApp() override { return new AppLauncher; }
            void deleteApp(void* app) override { delete (AppLauncher*)app; }
        };
    } // namespace APPS
} // namespace MOONCAKE
