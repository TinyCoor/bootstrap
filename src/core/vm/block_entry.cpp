//
// Created by 12132 on 2022/8/5.
//

#include "block_entry.h"
namespace gfx {

block_entry_t::block_entry_t()
    : data_({}), type_(block_entry_type_t::blank_line)
{
}

block_entry_t::block_entry_t(const align_t& align)
    : data_(std::any(align)), type_(block_entry_type_t::align)
{

}

block_entry_t::block_entry_t(const label_t& label)
    : data_(std::any(label)), type_(block_entry_type_t::label)
{

}

block_entry_t::block_entry_t(const comment_t& comment)
    : data_(std::any(comment)), type_(block_entry_type_t::comment)
{

}


block_entry_t::block_entry_t(const section_t& section)
    : data_(std::any(section)), type_(block_entry_type_t::section)
{
}

block_entry_t::block_entry_t(const data_definition_t& data)
    : data_(std::any(data)), type_(block_entry_type_t::data_definition)
{
}

block_entry_t::block_entry_t(const instruction_t& instruction)
    : data_(std::any(instruction)), type_(block_entry_type_t::instruction)
{
}

block_entry_t::block_entry_t(const block_entry_t& other)
    : data_(other.data_), address_(other.address_), type_(other.type_)
{
}

block_entry_t *block_entry_t::address(uint64_t value)
{
    address_ = value;
    if (type_ == block_entry_type_t::label) {
        auto label = data<label_t>();
        label->instance->address(value);
    }
    return this;
}

block_entry_type_t block_entry_t::type() const
{
    return type_;
}

}