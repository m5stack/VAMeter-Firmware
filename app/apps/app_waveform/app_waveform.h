/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#include "view/view.h"
#include <mooncake.h>

namespace MOONCAKE
{
    namespace APPS
    {
        /**
         * @brief Waveform
         *
         */
        class AppWaveform : public APP_BASE
        {
        private:
            struct Data_t
            {
                VIEWS::WaveFormRecorder* view = nullptr;
            };
            Data_t _data;
            void _handle_recording_finished();

        public:
            void onResume() override;
            void onRunning() override;
            void onDestroy() override;
        };

        class AppWaveform_Packer : public APP_PACKER_BASE
        {
            const char* getAppName() override;
            void* getAppIcon() override;
            void* newApp() override { return new AppWaveform; }
            void deleteApp(void* app) override { delete (AppWaveform*)app; }
            void* getCustomData() override;
        };
    } // namespace APPS
} // namespace MOONCAKE
