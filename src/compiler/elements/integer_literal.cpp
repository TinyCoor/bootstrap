//
// Created by 12132 on 2022/4/23.
//

#include "integer_literal.h"
namespace gfx::compiler {
integer_literal::integer_literal(uint64_t value)
	: value_(value)
{
}

integer_literal::~integer_literal()
{
}
}