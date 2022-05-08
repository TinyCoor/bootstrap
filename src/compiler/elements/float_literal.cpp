//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
namespace gfx::compiler {
float_literal::float_literal(element* parent,double value)
	: element(parent, element_type_t::float_literal), value_(value)
{
}

double float_literal::value() const {
	return value_;
}

}