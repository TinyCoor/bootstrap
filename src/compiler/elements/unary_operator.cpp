//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
namespace gfx::compiler {
unary_operator::unary_operator(element* parent, operator_type_t type,expression* rhs)
	: operator_base(parent,type), rhs_(rhs)
{
}

unary_operator::~unary_operator()
{
}

expression* unary_operator::rhs()
{
	return rhs_;
}
}