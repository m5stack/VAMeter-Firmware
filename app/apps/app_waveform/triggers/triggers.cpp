/**
 * @file triggers.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "triggers.h"

/* -------------------------------------------------------------------------- */
/*                              Trigger by level                              */
/* -------------------------------------------------------------------------- */
// 2000ms pre buffer
size_t Trigger_Level::preTriggerBufferSize() { return (2000 / getSampleInterval()); }

bool Trigger_Level::onCheck(VA_RECORDER::PreTriggerBuffer* preTriggerBuffer)
{
    bool ret = false;

    int trigger_count = 0;
    constexpr int max_trigger_count = 200 / 40;

    float value = 0.0f;
    preTriggerBuffer->peekAllWithBreak([&](const VA_RECORDER::RecordData_t& recordData, bool& stopPeeking) {
        // Get value
        if (getTriggerChannel())
            value = recordData.voltage;
        else
            value = recordData.current;

        // Check value
        if (_is_above_level)
        {
            if (value >= getThreshold())
                trigger_count++;
        }
        else
        {
            if (value <= getThreshold())
                trigger_count++;
        }

        // Trigger
        if (trigger_count >= max_trigger_count)
        {
            stopPeeking = true;
            ret = true;
        }
    });

    return ret;
}

/* -------------------------------------------------------------------------- */
/*                               Trigger by edge                              */
/* -------------------------------------------------------------------------- */
// 2000ms pre buffer
size_t Trigger_Edge::preTriggerBufferSize() { return (2000 / getSampleInterval()); }

bool Trigger_Edge::onCheck(VA_RECORDER::PreTriggerBuffer* preTriggerBuffer) { return false; }
