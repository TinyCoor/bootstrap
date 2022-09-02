//
// Created by 12132 on 2022/9/2.
//

#ifndef BOOTSTRAP_SRC_COMMON_RUNE_H_
#define BOOTSTRAP_SRC_COMMON_RUNE_H_
#include <cstdint>

using rune_t = int32_t;

inline static rune_t rune_invalid = 0xfffd;
inline static rune_t rune_max     = 0x0010ffff;
inline static rune_t rune_bom     = 0xfeff;
inline static rune_t rune_eof     = -1;
#endif //BOOTSTRAP_SRC_COMMON_RUNE_H_
