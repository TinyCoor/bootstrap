//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
namespace gfx::compiler {
float_literal::float_literal(element* parent,double value) : element(parent), value_(value)
{
}

float_literal::~float_literal()
{
}

}