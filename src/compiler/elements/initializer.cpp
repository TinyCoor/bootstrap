//
// Created by 12132 on 2022/4/23.
//

#include "initializer.h"
#include "binary_operator.h"

namespace gfx::compiler {

initializer::initializer( element* parent, binary_operator* assignment)
	: expression(parent,dynamic_cast<element*>(assignment))
{

}

initializer::~initializer()
{
}

binary_operator* initializer::assignment()
{
	return assignment_;
}
}
