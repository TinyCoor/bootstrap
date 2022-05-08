//
// Created by 12132 on 2022/4/23.
//

#include "namespace_element.h"
namespace gfx::compiler {
namespace_element::namespace_element(block* parent, const std::string& name)
	: block(parent,element_type_t::namespace_e), name_(name)
{
}

std::string namespace_element::name() const
{
	return name_;
}
}