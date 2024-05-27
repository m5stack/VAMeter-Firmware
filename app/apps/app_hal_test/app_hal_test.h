/**
 * @file app_hal_test.h
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

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Hal_test
         *
         */
        class AppHal_test : public APP_BASE
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

        class AppHal_test_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override { return "HalTest"; }
            void* getAppIcon() override;
            void* newApp() override { return new AppHal_test; }
            void deleteApp(void* app) override { delete (AppHal_test*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE