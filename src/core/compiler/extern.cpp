//
// Created by 12132 on 2022/7/16.
//

#include "extern.h"
#include <cstdarg>
#include <cstdio>
void print(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}