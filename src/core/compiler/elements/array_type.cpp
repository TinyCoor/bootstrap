//
// Created by 12132 on 2022/5/2.
//

#include "array_type.h"
namespace gfx::compiler {
array_type::array_type(element* parent, const std::string& name, compiler::type* entry_type, size_t size)
	:   composite_type(parent,composite_types_t::struct_type, name, element_type_t::array_type),
        size_(size), entry_type_(entry_type)
{
}

uint64_t array_type::size() const
{
	return size_;
}

void array_type::size(uint64_t value)
{
	size_ = value;
}

compiler::type* array_type::entry_type()
{
	return entry_type_;
}

bool array_type::on_initialize(result &r, compiler::program* program)
{
	return true;
}
}
