//
// Created by 12132 on 2022/5/8.
//

#include "alias.h"
namespace gfx::compiler {

alias::alias(block* parent, element* expr)
	: element(parent, element_type_t::alias_type), expression_(expr)
{

}

element* alias::expression()
{
	return expression_;
}

void alias::on_owned_elements(element_list_t &list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}
}