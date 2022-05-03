//
// Created by 12132 on 2022/4/23.
//

#include "identifier.h"
namespace gfx::compiler {
identifier::identifier(element *parent, const std::string& name, compiler::initializer* initializer)
	: element(parent), name_(name), initializer_(initializer) {
}

identifier::~identifier() {
}

compiler::type* identifier::type()
{
	return type_;
}

std::string identifier::name() const
{
	return name_;
}

bool identifier::is_constant() const
{
	return constant_;
}

compiler::initializer* identifier::initializer() const
{
	return initializer_;
}

bool identifier::inferred_type() const
{
	return inferred_type_;
}

void identifier::inferred_type(bool value)
{
	inferred_type_ = value;
}

void identifier::type(compiler::type* t)
{
	type_ = t;
}

void identifier::constant(bool value)
{
	constant_ =value;
}

}