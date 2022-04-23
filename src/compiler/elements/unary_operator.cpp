//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
namespace gfx::compiler {
unary_operator::unary_operator(operator_type_t type,element* rhs)
	: operator_base(type), rhs_(rhs) {
}

unary_operator::~unary_operator() {
}

element* unary_operator::rhs() {
	return rhs_;
}
}