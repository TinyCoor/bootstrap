//
// Created by 12132 on 2022/5/8.
//

#include "alias.h"
namespace gfx::compiler {

alias::alias(element* parent, element* expr)
	: element(parent, element_type_t::alias_type), expression_(expr)
{

}

element* alias::expression() {
	return expression_;
}
}