//
// Created by 12132 on 2022/4/23.
//

#include "initializer.h"
#include "binary_operator.h"
#include "core/compiler/elements/types/procedure_type.h"

namespace gfx::compiler {

initializer::initializer(block* parent, element* expr)
	: element(parent, element_type_t::initializer), expr_(expr)
{

}

element *initializer::expression()
{
	return expr_;
}

compiler::procedure_type *initializer::procedure_type()
{
	if (expr_ ==nullptr || expr_->element_type() != element_type_t::proc_type) {
		return nullptr;
	}
	return dynamic_cast<class procedure_type*>(expr_);
}

bool initializer::on_as_integer(uint64_t &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_integer(value);
}

bool initializer::on_as_string(std::string &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_string(value);
}

bool initializer::on_as_bool(bool &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_bool(value);
}

bool initializer::on_as_float(double &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_float(value);
}

bool initializer::on_emit(result &r, emit_context_t &context)
{
    if (expr_ == nullptr)
        return true;

    if (expr_->element_type() == element_type_t::namespace_e) {
        expr_->emit(r, context);
    }
    return true;
}
void initializer::expression(element *value)
{
    expr_ = value;
}

void initializer::on_owned_elements(element_list_t &list)
{
    if (expr_ != nullptr) {
        list.emplace_back(expr_);
    }
}

}
