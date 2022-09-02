//
// Created by 12132 on 2022/9/2.
//

#include "source_location.h"
const location_t &source_location::end() const
{
    return end_;
}

const location_t &source_location::start() const
{
    return start_;
}

void source_location::end(const location_t &value)
{
    end_ = value;
}

void source_location::start(const location_t &value)
{
    start_ = value;
}

void source_location::end(uint32_t line, uint32_t column)
{
    end_.line = line;
    end_.column = column;
}

void source_location::start(uint32_t line, uint32_t column)
{
    start_.line = line;
    start_.column = column;
}
