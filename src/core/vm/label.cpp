//
// Created by 12132 on 2022/7/17.
//

#include "label.h"
namespace gfx {
label::label(const std::string& name)
    : name_(name)
{
}

std::string label::name() const
{
    return name_;
}

uint64_t label::address() const
{
    return address_;
}

void label::address(uint64_t value)
{
    address_ = value;
}

}