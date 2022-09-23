//
// Created by 12132 on 2022/9/2.
//

#ifndef COMMON_RUNE_H_
#define COMMON_RUNE_H_
#include <cstdint>
using rune_t = int32_t;

static constexpr rune_t rune_invalid = 0xfffd;
static constexpr rune_t rune_max     = 0x0010ffff;
static constexpr rune_t rune_bom     = 0xfeff;
static constexpr rune_t rune_eof     = -1;

#endif // COMMON_RUNE_H_
