//
// Created by 12132 on 2022/7/17.
//

#include "segment.h"
namespace gfx{

segment::segment(const std::string &name, segment_type_t type)
    :   name_(name), type_(type)
{}


symbol *segment::symbol(const std::string &name)
{
    auto it = symbols_.find(name);
    if (it == symbols_.end()) {
        return nullptr;
    }
    return &it->second;
}

size_t segment::size() const
{
    return offset_;
}

symbol* segment::symbol(const std::string &name, symbol_type_t type, size_t size)
{
    auto type_size = size == 0 ? sizeof_symbol_type(type) : size;
    symbols_.insert(std::make_pair(name, gfx::symbol(name, type, offset_, type_size)));
    offset_ += type_size;
    return symbol(name);
}

symbol_list_t segment::symbols()
{
    symbol_list_t list{};
    for (const auto& it : symbols_) {
        list.emplace_back(it.second);
    }
    return list;
}

std::string segment::name() const
{
    return name_;
}

segment_type_t segment::type() const
{
    return type_;
}

bool segment::initialize() const
{
    return initialized;
}

void segment::initialize(bool value)
{
    initialized = value;
}

}