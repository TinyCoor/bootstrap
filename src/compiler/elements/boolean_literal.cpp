//
// Created by 12132 on 2022/5/2.
//

#include "boolean_literal.h"
namespace gfx::compiler{
boolean_literal::boolean_literal(const std::string &name, bool value)
	: type(name) ,value_(value)
{

}
}