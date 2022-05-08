//
// Created by 12132 on 2022/4/23.
//

#include "initializer.h"
#include "binary_operator.h"

namespace gfx::compiler {

initializer::initializer( element* parent, element* expr)
	: element(parent, element_type_t::initializer), expr_(expr)
{

}
element *initializer::expression()
{
	return expr_;
}

}
