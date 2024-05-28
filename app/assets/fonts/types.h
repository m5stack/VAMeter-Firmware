/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include <cstdint>
// Font subset gen by:
// https://forairaaaaa.github.io/lgfxFontSubsetGenerator/
// https://forairaaaaa.github.io/lgfxFontSubsetGenerator/ja-ver.html

/* -------------------------------------------------------------------------- */
/*                                    Fonts                                   */
/* -------------------------------------------------------------------------- */
struct FontPool_t
{
    // vlw
    uint8_t montserrat_semibold_16[27666];
    uint8_t montserrat_semibold_24[24428];
    uint8_t montserrat_semibold_36[49923];
    uint8_t montserrat_semibolditalic_14[24465];
    uint8_t montserrat_semibolditalic_24[56523];
    uint8_t montserrat_semibolditalic_72[627831];

    // efont
    uint8_t efontCN_16_subset_data[8160];
    uint8_t efontCN_24_subset_data[13264];
    uint8_t efontJA_16_subset_data[9170];
    uint8_t efontJA_24_subset_data[14717];
};
