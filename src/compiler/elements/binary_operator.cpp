//
// Created by 12132 on 2022/4/23.
//

#include "binary_operator.h"
namespace gfx::compiler {
binary_operator::binary_operator(operator_type_t type, element* lhs, element* rhs)
	: operator_base(type), lhs_(lhs), rhs_(rhs)
{
}

binary_operator::~binary_operator()
{
}

element* binary_operator::lhs()
{
	return lhs_;
}

element* binary_operator::rhs()
{
	return rhs_;
}
}