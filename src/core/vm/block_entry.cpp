//
// Created by 12132 on 2022/8/5.
//

#include "block_entry.h"
namespace gfx {

block_entry_t::block_entry_t()
    : data_({}), type_(block_entry_type_t::memo)
{
}

block_entry_t::block_entry_t(const align_t& align)
    : data_(std::any(align)), type_(block_entry_type_t::align)
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
    : data_(other.data_), address_(other.address_), type_(other.type_),
      blank_lines_(other.blank_lines_), comments_(other.comments_), labels_(other.labels_)
{
}

block_entry_t *block_entry_t::label(class label *label)
{
    labels_.emplace_back(label);
    return this;
}

block_entry_type_t block_entry_t::type() const
{
    return type_;
}

const std::vector<class label *> &block_entry_t::labels() const
{
    return labels_;
}

block_entry_t *block_entry_t::comment(const std::string& value, uint8_t indent)
{
    comments_.push_back(comment_t {
        .indent = indent,
        .value = value
    });
    return this;
}

comment_list_t block_entry_t::comments()
{
    return comments_;
}
}