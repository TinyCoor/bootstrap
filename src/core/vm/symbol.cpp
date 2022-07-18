//
// Created by 12132 on 2022/7/17.
//

#include "symbol.h"
namespace gfx {
symbol::symbol(const std::string& name,  symbol_type_t type, uint64_t offset, size_t size)
    :  offset_(offset), name_(name) ,type_(type), size_(size)
{

}

std::string symbol::name() const
{
    return name_;
}

size_t symbol::size() const
{
    return size_;
}

symbol_type_t symbol::type() const
{
    return type_;
}

uint64_t symbol::offset() const
{
    return offset_;
}
void symbol::value(uint64_t value)
{
    value_.int_value = value;
}
void symbol::value(double value)
{
    value_.float_value = value;
}

void symbol::value(void *v)
{
    value_.byte_array_value = v;
}

id_t symbol::pending_address_from_id() const
{
    return pending_address_from_id_;
}

void symbol::pending_address_from_id(id_t value)
{
    pending_address_from_id_ = value;
}

}