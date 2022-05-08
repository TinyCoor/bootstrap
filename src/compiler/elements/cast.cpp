//
// Created by 12132 on 2022/5/8.
//

#include "cast.h"
namespace gfx::compiler {
cast::cast(element* parent, compiler::type* type, element* expr)
	: element(parent, element_type_t::cast),
					 expression_(expr),
					 type_(type) {
}

element* cast::expression() {
	return expression_;
}

compiler::type* cast::type() {
	return type_;
}

}