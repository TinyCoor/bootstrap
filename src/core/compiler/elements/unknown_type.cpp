//
// Created by 12132 on 2022/5/29.
//

#include "unknown_type.h"
gfx::compiler::unknown_type::unknown_type(gfx::compiler::element *parent, const std::string &name,
   bool is_array, size_t size)
	: type(parent, element_type_t::unknown_type, name),  array_size_(size), is_array_(is_array)
{

}
bool gfx::compiler::unknown_type::is_array() const
{
	return false;
}
void gfx::compiler::unknown_type::is_array(bool value)
{

}
size_t gfx::compiler::unknown_type::array_size() const
{
	return 0;
}
void gfx::compiler::unknown_type::array_size(size_t value)
{

}
bool gfx::compiler::unknown_type::on_initialize(gfx::result &r)
{
	return true;
}
