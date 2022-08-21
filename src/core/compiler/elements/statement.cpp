//
// Created by 12132 on 2022/5/8.
//
#include "label.h"
#include "statement.h"
namespace gfx::compiler {
statement::statement(block* parent, element* expr)
	: element(parent, element_type_t::statement),
					 expression_(expr) {
}

element* statement::expression() {
	return expression_;
}

label_list_t& statement::labels()
{
	return labels_;
}

bool statement::on_emit(result &r, emit_context_t &context)
{
    if (expression_ == nullptr) {
        return true;
    }

    //
    // need to loop over labels and add them to the assembler here
    //
    return expression_->emit(r, context);
}

void statement::on_owned_elements(element_list_t &list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
    for (auto element : labels_) {
        list.emplace_back(element);
    }
}
}