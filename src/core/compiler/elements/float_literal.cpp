//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
#include "program.h"
namespace gfx::compiler {
float_literal::float_literal(element* parent,double value)
	: element(parent, element_type_t::float_literal), value_(value)
{
}

double float_literal::value() const
{
	return value_;
}

compiler::type *float_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type("f64");
}

}