/**
 * @file encoder.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "../../../../../hal/hal.h"
#include <cstdint>

namespace SYSTEM
{
    namespace INPUTS
    {
        class Encoder
        {
        private:
            static Encoder* _encoder;

        public:
            static Encoder* Get();

        private:
            struct Data_t
            {
                int last_encoder_count = 0;
                int encoder_direction = 0;
            };
            Data_t _data;

        public:
            static void Update() { Get()->update(); }
            void update();

            static void Reset() { Get()->reset(); }
            void reset();

            static int GetCount() { return Get()->getCount(); }
            inline int getCount() { return _data.last_encoder_count; }

            static int GetDirection() { return Get()->getDirection(); }
            inline int getDirection() { return _data.encoder_direction; }

            static bool WasMoved() { return Get()->wasMoved(); }
            inline bool wasMoved() { return _data.encoder_direction != 0; }
        };
    } // namespace INPUTS
} // namespace SYSTEM
