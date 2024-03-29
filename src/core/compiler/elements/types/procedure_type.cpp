//
// Created by 12132 on 2022/5/2.
//
#include "core/compiler/elements/program.h"
#include "core/compiler/session.h"
#include "procedure_type.h"
#include "../symbol_element.h"
#include "fmt/core.h"
namespace gfx::compiler {
procedure_type::procedure_type(compiler::module* module, block* parent_scope,
                               compiler::block* scope, symbol_element* symbol)
	: type(module, parent_scope, element_type_t::proc_type, symbol), scope_(scope)
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

bool procedure_type::on_initialize(compiler::session& session)
{
	return true;
}

bool procedure_type::on_emit(compiler::session &session)
{
    auto &assembler = session.assembler();
    auto procedure_label = symbol()->name();
    auto parent_init = parent_element_as<compiler::initializer>();
    if (parent_init != nullptr) {
        auto parent_var = parent_init->parent_element_as<compiler::identifier>();
        if (parent_var != nullptr) {
            procedure_label = parent_var->symbol()->name();
        }
    }

    if (is_foreign()) {
        return true;
    }

    auto stack_frame = session.stack_frame();

    auto instruction_block = assembler.make_procedure_block();
    instruction_block->align(instruction_t::alignment);
    instruction_block->blank_line();

    instruction_block->label(assembler.make_label(procedure_label));

    int32_t offset = -8;
    for (auto param : parameters_.as_list()) {
        stack_frame->add(stack_frame_entry_type_t::parameter, param->identifier()->symbol()->name(), offset);
        offset -= 8;
    }

    offset = 8;
    const auto &return_list = returns_.as_list();
    if (!return_list.empty()) {
        stack_frame->add(stack_frame_entry_type_t::return_slot, "return_value", offset);
        offset += 8;
    }
    offset = 16;
    size_t local_count = 0;
    session.scope_manager().visit_blocks(session.result(), [&](compiler::block* scope) {
          if (scope->element_type() == element_type_t::proc_type_block) {
              return true;
          }
          for (auto var : scope->identifiers().as_list()) {
              if (var->type()->element_type() == element_type_t::proc_type) {
                  continue;
              }
              stack_frame->add(stack_frame_entry_type_t::local,  var->symbol()->name(), offset);
              var->usage(identifier_usage_t::stack);
              offset += 8;
              local_count++;
          }
          return true;
    }, scope_);

    instruction_block->move_reg_to_reg(register_t::fp(), register_t::sp());
    auto size =  8 * local_count;
    if (!return_list.empty()) {
        size += 8;
    }
    if (size > 0) {
        instruction_block->sub_reg_by_immediate(register_t::sp(), register_t::sp(), size);
    }

    assembler.push_block(instruction_block);
    scope_->emit(session);
    assembler.pop_block();

    return true;
}

bool procedure_type::on_is_constant() const
{
    return true;
}
void procedure_type::on_owned_elements(element_list_t &list)
{
    if (scope_ != nullptr) {
        list.emplace_back(scope_);
    }
    for (auto element : returns_.as_list()) {
        list.emplace_back(element);
    }

    for (auto element : parameters_.as_list()) {
        list.emplace_back(element);
    }

}
uint64_t procedure_type::foreign_address() const
{
    return foreign_address_;
}

void procedure_type::foreign_address(uint64_t value)
{
    foreign_address_ = value;
}

type_access_model_t procedure_type::on_access_model() const
{
    return type_access_model_t::pointer;
}

bool procedure_type::on_type_check(compiler::type *other)
{
    if (other == nullptr) {
        return false;
    }
    return symbol()->name() == other->symbol()->name();
}

}