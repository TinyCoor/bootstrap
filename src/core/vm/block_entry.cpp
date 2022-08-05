//
// Created by 12132 on 2022/8/5.
//

#include "block_entry.h"
namespace gfx {
block_entry_t::block_entry_t(const section_t& section)
    : data_(std::any(section)), type_(block_entry_type_t::section)
{
}

block_entry_t::block_entry_t(const instruction_t& instruction)
    : data_(std::any(instruction)), type_(block_entry_type_t::instruction)
{
}

block_entry_t::block_entry_t(const data_definition_t& data)
    : data_(std::any(data)), type_(block_entry_type_t::data_definition)
{
}

void block_entry_t::label(class label *label)
{
    labels_.emplace_back(label);
}

block_entry_type_t block_entry_t::type() const
{
    return type_;
}

void block_entry_t::comment(const std::string &value)
{
    comments_.emplace_back(value);
}

const std::vector<class label *> &block_entry_t::labels() const
{
    return labels_;
}

const std::vector<std::string> &block_entry_t::comments() const
{
    return comments_;
}
}