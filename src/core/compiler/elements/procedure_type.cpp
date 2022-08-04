//
// Created by 12132 on 2022/5/2.
//
#include "program.h"
#include "procedure_type.h"
#include "procedure_type.h"
namespace gfx::compiler {
procedure_type::procedure_type(block* parent, compiler::block* scope, const std::string& name)
	: type(parent, element_type_t::proc_type, name), scope_(scope)
{

}

field_map_t& procedure_type::returns()
{
	return returns_;
}

field_map_t& procedure_type::parameters()
{
	return parameters_;
}

[[maybe_unused]] type_map_t& procedure_type::type_parameters()
{
	return type_parameters_;
}

procedure_instance_list_t& procedure_type::instances()
{
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

bool procedure_type::on_emit(result &r, emit_context_t &context)
{
    if (is_foreign())  {
        return true;
    }

    auto assembler = context.assembler;
    auto instruction_block = assembler->make_procedure_block();
    auto procedure_label = name();

    auto parent_init = parent_element_as<compiler::initializer>();
    if (parent_init !=nullptr) {
        auto parent_var = parent_init->parent_element_as<compiler::identifier>();
        if (parent_var != nullptr) {
            procedure_label = parent_var->name();
        }
    }
    instruction_block->make_label(procedure_label);

    auto stack_frame = instruction_block->stack_frame();
    int32_t offset = -8;
    for (auto param : parameters_.as_list()) {
        stack_frame->add(stack_frame_entry_type_t::parameter, param->identifier()->name(), offset);
        offset -= 8;
    }

    offset = 8;
    for (auto return_param : returns_.as_list()) {
        (void)return_param;
        stack_frame->add(stack_frame_entry_type_t::return_slot, "return_value", offset);
        offset += 8;
    }

    offset = 16;
    size_t local_count = 0;
    context.program->visit_blocks(r, [&](compiler::block* scope) {
          if (scope->element_type() == element_type_t::proc_type_block) {
              return true;
          }
          for (auto var : scope->identifiers().as_list()) {
              stack_frame->add(stack_frame_entry_type_t::local,  var->name(), offset);
              offset += 8;
              local_count++;
          }
          return true;
        }, scope_);

    instruction_block->move_ireg_to_ireg(i_registers_t::fp, i_registers_t::sp);
    if (local_count > 0) {
        instruction_block->sub_ireg_by_immediate(i_registers_t::sp, i_registers_t::sp,
            8 * local_count);
    }

    context.assembler->push_block(instruction_block);
    scope_->emit(r, context);
    assembler->pop_block();

    return true;
}

bool procedure_type::on_is_constant() const
{
    return true;
}

}