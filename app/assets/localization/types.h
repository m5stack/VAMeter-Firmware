/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/
#pragma once
#include "text_pool/text_pool_map.h"
#include "text_pool/text_pool_en.h"
#include "text_pool/text_pool_cn.h"
#include "text_pool/text_pool_jp.h"

/* -------------------------------------------------------------------------- */
/*                                  Text pool                                 */
/* -------------------------------------------------------------------------- */
struct TextPool_t
{
    TextPool_en_t TextEN;
    TextPool_cn_t TextCN;
    TextPool_jp_t TextJP;
};
