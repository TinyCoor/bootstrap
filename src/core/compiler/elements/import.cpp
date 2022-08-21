//
// Created by 12132 on 2022/7/2.
//

#include "import.h"
namespace gfx::compiler {
import::import(block* parent, element* expr)
	: element(parent, element_type_t::import_e), expression_(expr)
{
}

element* import::expression()
{
	return expression_;
}

void import::on_owned_elements(element_list_t &list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}
}