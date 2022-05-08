//
// Created by 12132 on 2022/5/3.
//

#include "any_type.h"
namespace gfx::compiler {
any_type::any_type(element* parent)
	: type(parent, element_type_t::any_type, "any")
{

}

}