//
// Created by 12132 on 2022/4/23.
//

#include "namespace_element.h"
#include "program.h"
namespace gfx::compiler {
namespace_element::namespace_element(element *parent, element *expression)
	: element(parent,element_type_t::namespace_e), expression_(expression)
{
}

element *namespace_element::expression()
{
	return expression_;
}

compiler::type *namespace_element::on_infer_type(const compiler::program *program)
{
	return program->find_type("namespace");
}

bool compiler::namespace_element::on_is_constant() const
{
    return true;

}

bool namespace_element::on_emit(result &r, emit_context_t &context)
{
    if (expression_ == nullptr) {
        return true;
    }

    return expression_->emit(r, context);
}
}