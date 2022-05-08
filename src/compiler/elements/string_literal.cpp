//
// Created by 12132 on 2022/4/23.
//

#include "string_literal.h"
namespace gfx::compiler {
string_literal::string_literal(element* parent, const std::string& value)
	: element(parent, element_type_t::string_literal),value_(value)
{
}

}