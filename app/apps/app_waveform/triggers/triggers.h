/**
 * @file triggers.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2024-03-19
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include "../../../hal/hal.h"
#include <cstdint>
#include <functional>

/* -------------------------------------------------------------------------- */
/*                                Trigger mode                                */
/* -------------------------------------------------------------------------- */
enum TriggerMode_t
{
    trigger_mode_manual = 0,
    trigger_mode_v_above_level,
    trigger_mode_v_under_level,
    trigger_mode_a_above_level,
    trigger_mode_a_under_level,
    trigger_mode_num,
};

inline bool IsTriggerModeHasThreshold(TriggerMode_t mode)
{
    if (mode == trigger_mode_manual)
        return false;
    return true;
}

inline bool IsTriggerModeChannelV(TriggerMode_t mode)
{
    if (mode == trigger_mode_v_above_level)
        return true;
    if (mode == trigger_mode_v_under_level)
        return true;
    return false;
}

/* -------------------------------------------------------------------------- */
/*                              Trigger manually                              */
/* -------------------------------------------------------------------------- */
class Trigger_Manual : public VA_RECORDER::TriggerBase
{
};

/* -------------------------------------------------------------------------- */
/*                              Trigger by level                              */
/* -------------------------------------------------------------------------- */
class Trigger_Level : public VA_RECORDER::TriggerBase
{
private:
    bool _is_above_level;

public:
    Trigger_Level(bool aboveLevel) : _is_above_level(aboveLevel) { setHasThreshold(true); }
    size_t preTriggerBufferSize() override;
    bool onCheck(VA_RECORDER::PreTriggerBuffer* preTriggerBuffer) override;
};

/* -------------------------------------------------------------------------- */
/*                               Trigger by edge                              */
/* -------------------------------------------------------------------------- */
class Trigger_Edge : public VA_RECORDER::TriggerBase
{
private:
    bool _is_rising_edge;

public:
    Trigger_Edge(bool risingEdge) : _is_rising_edge(risingEdge) { setHasThreshold(true); }
    size_t preTriggerBufferSize() override;
    bool onCheck(VA_RECORDER::PreTriggerBuffer* preTriggerBuffer) override;
};
