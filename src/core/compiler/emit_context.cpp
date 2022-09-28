//
// Created by 12132 on 2022/7/23.
//

#include "emit_context.h"
#include "elements/types/type.h"
#include "elements/identifier.h"
#include "elements/symbol_element.h"
#include "elements/string_literal.h"
#include "elements/identifier_reference.h"

#include "compiler_types.h"
namespace gfx::compiler {
emit_context_t::emit_context_t(class terp* terp, class assembler* assembler, compiler::program* program)
    : terp(terp), assembler(assembler), program(program)
{

}

void emit_context_t::push_if(const std::string &true_label_name, const std::string &false_label_name)
{
    data_stack.push(std::any(if_data_t{
        .true_branch_label = true_label_name,
        .false_branch_label = false_label_name,
    }));
}

bool emit_context_t::has_scratch_register() const
{
    return !scratch_registers.empty();
}

[[maybe_unused]] void emit_context_t::clear_scratch_registers()
{
    while (!scratch_registers.empty()) {
        scratch_registers.pop();
    }
}

i_registers_t emit_context_t::pop_scratch_register()
{
    if (scratch_registers.empty()) {
        return i_registers_t::i0;
    }
    auto reg = scratch_registers.top();
    scratch_registers.pop();
    return reg;
}

void emit_context_t::push_scratch_register(i_registers_t reg)
{
    scratch_registers.push(reg);
}
void emit_context_t::pop()
{
    if (data_stack.empty()) {
        return;
    }
    data_stack.pop();
}

void emit_context_t::free_variable(const std::string &name)
{
    auto var = variable(name);
    if (var != nullptr) {
        var->make_dormat(assembler);
    }
}
variable_t *emit_context_t::variable(const std::string &name)
{
    const auto it = variables.find(name);
    if (it == variables.end()) {
        return nullptr;
    }
    return &it->second;
}

variable_t *emit_context_t::allocate_variable(result &r,  const std::string &name, compiler::type *type,
    identifier_usage_t usage, stack_frame_entry_t *frame_entry)
{
    auto var = variable(name);
    if (var != nullptr) {
        return var;
    }
    auto it = variables.insert(std::make_pair(
        name,
        variable_t {
            .name = name,
            .written = false,
            .requires_read = false,
            .address_loaded = false,
            .usage = usage,
            .type = type,
            .frame_entry = frame_entry,
        }));
    return it.second ? &it.first->second : nullptr;
}

compiler::variable_t *compiler::emit_context_t::variable_for_element(gfx::compiler::element *element)
{
    if (element == nullptr)
        return nullptr;
    switch (element->element_type()) {
        case element_type_t::identifier: {
            auto identifier = dynamic_cast<compiler::identifier*>(element);
            return variable(identifier->symbol()->name());
        }
        case element_type_t::string_literal: {
            auto string_literal = dynamic_cast<compiler::string_literal*>(element);
            return variable(string_literal->label_name());
        }
        case element_type_t::identifier_reference: {
            auto identifier = dynamic_cast<compiler::identifier_reference*>(element)->identifier();
            return variable(identifier->symbol()->name());
        }
        default:
            return nullptr;
    }
}

}