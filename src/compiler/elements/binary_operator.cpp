//
// Created by 12132 on 2022/4/23.
//

#include "binary_operator.h"
namespace gfx::compiler {
binary_operator::binary_operator(element* parent, operator_type_t type, expression* lhs, expression* rhs)
	: operator_base(parent, type), lhs_(lhs), rhs_(rhs)
{
}

binary_operator::~binary_operator()
{
}

expression* binary_operator::lhs()
{
	return lhs_;
}

expression* binary_operator::rhs()
{
	return rhs_;
}
}