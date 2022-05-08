//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
namespace gfx::compiler {
unary_operator::unary_operator(element* parent, operator_type_t type,element* rhs)
	: operator_base(parent, element_type_t::unary_operator, type), rhs_(rhs)
{
}

element* unary_operator::rhs()
{
	return rhs_;
}
}