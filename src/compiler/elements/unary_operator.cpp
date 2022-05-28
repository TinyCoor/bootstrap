//
// Created by 12132 on 2022/4/23.
//

#include "unary_operator.h"
#include "program.h"
namespace gfx::compiler {
unary_operator::unary_operator(element* parent, operator_type_t type,element* rhs)
	: operator_base(parent, element_type_t::unary_operator, type), rhs_(rhs)
{
}

element* unary_operator::rhs()
{
	return rhs_;
}
/// TODO more
compiler::type *unary_operator::on_infer_type(const compiler::program *program)
{
	switch (operator_type()) {
		case operator_type_t::negate:
		case operator_type_t::binary_not: {
			return program->find_type("u64");
		}
		case operator_type_t::logical_not: {
			return program->find_type("bool");
		}
		default:
			return nullptr;
	}
}
}