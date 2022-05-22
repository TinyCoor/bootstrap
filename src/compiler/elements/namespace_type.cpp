//
// Created by 12132 on 2022/5/22.
//

#include "namespace_type.h"
namespace gfx::compiler{
namespace_type::namespace_type(element *parent)
	: type(parent, element_type_t::namespace_type, "namespace")
{}
}