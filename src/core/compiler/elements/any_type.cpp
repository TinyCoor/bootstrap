//
// Created by 12132 on 2022/5/3.
//

#include "any_type.h"
namespace gfx::compiler {
any_type::any_type(element * parent)
	: type(parent, element_type_t::any_type ,"any")
{

}

compiler::type *any_type::underlying_type()
{
	return underlying_type_;
}

void any_type::underlying_type(compiler::type *value)
{
	underlying_type_ = value;
}

bool any_type::on_initialize(result &r)
{
	return true;
}

}