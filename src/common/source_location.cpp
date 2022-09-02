//
// Created by 12132 on 2022/9/2.
//

#include "source_location.h"
source_location::source_location()
{

}

source_location::source_location(uint32_t line, uint16_t start_column, uint16_t end_column)
    : line_(line), end_column_(end_column), start_column_(start_column)
{

}
uint32_t source_location::line() const
{
    return line_;
}

void source_location::line(uint32_t value)
{
    line_ = value;
}

uint16_t source_location::end_column() const
{
    return end_column_;
}

uint16_t source_location::start_column() const
{
    return start_column_;
}

void source_location::end_column(uint16_t value)
{
    end_column_ = value;
}

void source_location::start_column(uint16_t value)
{
    start_column_ = value;
}
