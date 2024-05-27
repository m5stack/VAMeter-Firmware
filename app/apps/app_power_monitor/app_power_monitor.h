/**
 * @file app_power_monitor.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-02-26
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "view/view.h"
#include <cstdint>
#include <mooncake.h>

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Power_monitor
         *
         */
        class AppPower_monitor : public APP_BASE
        {
        private:
            struct Data_t
            {
                VIEWS::PmDataPage* view = nullptr;
                int current_page_num = 0;
                bool is_page_switched = false;
                uint32_t pm_update_time_count = 0;
            };
            Data_t _data;
            void _update_view();
            void _check_page_switch();
            void _setup_page_bus_volt();
            void _setup_page_bus_power();
            void _setup_page_shunt_current();
            void _setup_page_shunt_volt();
            void _setup_page_simple_detail();
            void _setup_page_more_detail();

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppPower_monitor_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override;
            void* getAppIcon() override;
            void* newApp() override { return new AppPower_monitor; }
            void deleteApp(void* app) override { delete (AppPower_monitor*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE