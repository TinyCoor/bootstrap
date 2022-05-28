//
// Created by 12132 on 2022/4/23.
//

#include "string_literal.h"
#include "program.h"
namespace gfx::compiler {
string_literal::string_literal(element* parent, const std::string& value)
	: element(parent, element_type_t::string_literal),value_(value)
{
}

std::string string_literal::value() const
{
	return value_;
}

compiler::type *string_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type("string");
}

}