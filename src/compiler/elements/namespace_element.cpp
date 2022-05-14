//
// Created by 12132 on 2022/4/23.
//

#include "namespace_element.h"
namespace gfx::compiler {
namespace_element::namespace_element(element *parent, element *expression)
	: element(parent,element_type_t::namespace_e), expression_(expression)
{
}

element *namespace_element::expression()
{
	return expression_;
}
}