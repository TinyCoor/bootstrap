//
// Created by 12132 on 2022/4/23.
//

#include "directive.h"
namespace gfx::compiler {
directive::directive(const std::string& name, expression* rhs)
	: name_(name), rhs_(rhs) {
}

directive::~directive() {
}

std::string directive::name() const {
	return name_;
}

compiler::expression* directive::rhs() {
	return rhs_;
}
}