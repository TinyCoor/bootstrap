//
// Created by 12132 on 2022/5/8.
//
#include "block.h"
#include "procedure_instance.h"
namespace gfx::compiler {
procedure_instance::procedure_instance(compiler::module* module, block* parent, compiler::type* procedure_type, block* scope)
	: element(module, parent, element_type_t::proc_instance), scope_(scope), procedure_type_(procedure_type)
{
}

block* procedure_instance::scope()
{
	return scope_;
}

compiler::type* procedure_instance::procedure_type()
{
	return procedure_type_;
}

bool procedure_instance::on_emit(compiler::session &session)
{
    return scope_->emit(session);
}

void procedure_instance::on_owned_elements(element_list_t &list)
{
    if (scope_ != nullptr) {
        list.emplace_back(scope_);
    }
}
}