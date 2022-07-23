//
// Created by 12132 on 2022/4/23.
//

#include "initializer.h"
#include "binary_operator.h"
#include "procedure_type.h"

namespace gfx::compiler {

initializer::initializer( element* parent, element* expr)
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

bool initializer::on_emit(result &r, assembler &assembler, const emit_context_t &context)
{
    return true;
}

}
