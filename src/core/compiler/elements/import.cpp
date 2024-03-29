//
// Created by 12132 on 2022/7/2.
//

#include "import.h"
namespace gfx::compiler {
import::import(compiler::module* module, block* parent, element* expr, element* from_expr, compiler::module* imported_module)
	: element(module, parent, element_type_t::import_e), imported_module_(imported_module),
        expression_(expr), from_expression_(from_expr)
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
    if (from_expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}
element *import::from_expression()
{
    return from_expression_ == nullptr ? nullptr : from_expression_;
}
compiler::module *import::imported_module()
{
    return nullptr;
}

}