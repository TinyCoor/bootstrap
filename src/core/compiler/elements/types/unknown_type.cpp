//
// Created by 12132 on 2022/5/29.
//

#include "unknown_type.h"
gfx::compiler::unknown_type::unknown_type(block* parent_scope, compiler::symbol_element* symbol)
	: type(parent_scope, element_type_t::unknown_type, symbol)
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
bool gfx::compiler::unknown_type::on_initialize(result &r, compiler::program* program)
{
	return true;
}