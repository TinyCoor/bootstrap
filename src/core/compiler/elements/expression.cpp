//
// Created by 12132 on 2022/4/23.
//

#include "expression.h"
namespace gfx::compiler {
expression::expression(compiler::module* module, block* parent, element* root)
	: element(module, parent, element_type_t::expression),root_(root)
{
}

element *expression::root()
{
	return root_;
}

bool expression::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
	if (root_ == nullptr) {
		return false;
	}
	return root_->infer_type(session, result);
}

bool expression::on_is_constant() const
{
    if (root_ == nullptr) {
        return false;
    }
    return root_->is_constant();
}

bool compiler::expression::on_emit(compiler::session& session)
{
    if (root_ == nullptr) {
        return true;
    }

    return root_->emit(session);
}
void expression::on_owned_elements(element_list_t &list)
{
    if (root_ != nullptr) {
        list.emplace_back(root_);
    }
}

}