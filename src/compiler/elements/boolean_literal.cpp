//
// Created by 12132 on 2022/5/2.
//

#include "boolean_literal.h"
#include "program.h"
namespace gfx::compiler{
boolean_literal::boolean_literal(element* parent, bool value)
	: element(parent, element_type_t::boolean_literal) ,value_(value)
{

}

bool boolean_literal::value() const
{
	return value_;
}

compiler::type *boolean_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type("bool");
}
}