//
// Created by 12132 on 2022/5/22.
//

#include "namespace_type.h"
namespace gfx::compiler{
namespace_type::namespace_type(element *parent)
	: type(parent, element_type_t::namespace_type, "namespace")
{

}

bool namespace_type::on_initialize(result &r, compiler::program* program)
{
	return true;
}
}