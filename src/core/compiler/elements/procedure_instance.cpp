//
// Created by 12132 on 2022/5/8.
//
#include "block.h"
#include "procedure_instance.h"
namespace gfx::compiler {
procedure_instance::procedure_instance(element* parent, compiler::type* procedure_type, block* scope)
	: element(parent, element_type_t::proc_instance), scope_(scope), procedure_type_(procedure_type)
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

bool procedure_instance::on_emit(result &r, emit_context_t& context)
{
    return scope_->emit(r, context);
}
}