//
// Created by 12132 on 2022/4/23.
//

#include "expression.h"
namespace gfx::compiler {
expression::expression(block* parent, element* root)
	: element(parent, element_type_t::expression),root_(root)
{
}

element *expression::root()
{
	return root_;
}

compiler::type *expression::on_infer_type(const compiler::program *program)
{
	if (root_ == nullptr) {
		return nullptr;
	}
	return root_->infer_type(program);
}

bool expression::on_is_constant() const
{
    if (root_ == nullptr) {
        return false;
    }
    return root_->is_constant();
}

bool compiler::expression::on_emit(gfx::result &r, emit_context_t& context)
{
    if (root_ == nullptr) {
        return true;
    }

    return root_->emit(r, context);
}
void expression::on_owned_elements(element_list_t &list)
{
    if (root_ != nullptr) {
        list.emplace_back(root_);
    }
}

}