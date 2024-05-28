/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>

/* -------------------------------------------------------------------------- */
/*                                  Web pages                                 */
/* -------------------------------------------------------------------------- */
struct WebPagePool_t
{
    uint8_t syscfg[80120];
    uint8_t favicon[5182];
};
