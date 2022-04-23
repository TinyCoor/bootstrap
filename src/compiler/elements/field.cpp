//
// Created by 12132 on 2022/4/23.
//

#include "field.h"
namespace gfx::compiler {
field::field(const std::string& name, compiler::type* type,
	compiler::initializer* initializer)
	: name_(name), type_(type), initializer_(initializer) {
}

field::~field() {
}

compiler::type* field::type() {
	return type_;
}

std::string field::name() const {
	return name_;
}

bool field::inferred_type() const {
	return inferred_type_;
}

void field::type(compiler::type* t) {
	type_ = t;
}

compiler::initializer* field::initializer() {
	return initializer_;
}

void field::initializer(compiler::initializer* v) {
	initializer_ = v;
}

}