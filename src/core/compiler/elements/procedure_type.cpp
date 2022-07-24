//
// Created by 12132 on 2022/5/2.
//
#include "program.h"
#include "procedure_type.h"
#include "procedure_type.h"
namespace gfx::compiler {
procedure_type::procedure_type(element* parent, compiler::block* scope, const std::string& name)
	: type(parent, element_type_t::proc_type, name), scope_(scope)
{

}

field_map_t& procedure_type::returns() {
	return returns_;
}

field_map_t& procedure_type::parameters() {
	return parameters_;
}

[[maybe_unused]] type_map_t& procedure_type::type_parameters() {
	return type_parameters_;
}

procedure_instance_list_t& procedure_type::instances() {
	return instances_;
}

compiler::block *procedure_type::scope()
{
	return scope_;
}
bool procedure_type::is_foreign() const
{
	return is_foreign_;
}

void procedure_type::is_foreign(bool value)
{
	is_foreign_ = value;
}

bool procedure_type::on_initialize(result &r, compiler::program* program)
{
	return true;
}

bool procedure_type::on_emit(result &r, assembler &assembler, const emit_context_t &context)
{
    if (is_foreign())  {
        return true;
    }

    auto instruction_block = assembler.make_procedure_block();
    auto proc_label = name();
    auto procedure_label = name();

    switch (context.type) {
        case emit_context_type_t::procedure_type:
            procedure_label = context.data.procedure_type->identifier_name;
            break;
        default:
            break;
    }
    instruction_block->make_label(procedure_label);
    assembler.push_block(instruction_block);
    scope_->emit(r, assembler, context);
    assembler.pop_block();

    return true;
}

bool procedure_type::on_is_constant() const
{
    return true;
}

}