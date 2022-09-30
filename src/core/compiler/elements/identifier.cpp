//
// Created by 12132 on 2022/4/23.
//

#include "identifier.h"
#include "types/type.h"
#include "initializer.h"
#include "fmt/format.h"
#include "symbol_element.h"
#include "vm/instruction_block.h"
namespace gfx::compiler {
identifier::identifier(block* parent_scope, symbol_element* symbol, compiler::initializer* initializer)
	: element(parent_scope, element_type_t::identifier), symbol_(symbol), initializer_(initializer)
{
}

compiler::type* identifier::type()
{
	return type_;
}

compiler::initializer* identifier::initializer() const
{
	return initializer_;
}

bool identifier::inferred_type() const
{
	return inferred_type_;
}

void identifier::inferred_type(bool value)
{
	inferred_type_ = value;
}

void identifier::type(compiler::type* t)
{
	type_ = t;
}

bool identifier::on_as_integer(uint64_t &value) const
{
    if (!initializer_) {
        return false;
    }
    return initializer_->as_integer(value);
}

bool identifier::on_as_string(std::string &value) const
{
    if (!initializer_) {
        return false;
    }
    return initializer_->as_string(value);
}

bool identifier::on_as_bool(bool &value) const
{
    if (!initializer_) {
        return false;
    }
    return initializer_->as_bool(value);
}

bool identifier::on_as_float(double &value) const
{
    if (!initializer_) {
        return false;
    }
    return initializer_->as_float(value);
}

bool identifier::on_emit(result &r, emit_context_t &context)
{
    if (type_->element_type() == element_type_t::namespace_type) {
        return true;
    }
    auto instruction_block = context.assembler->current_block();
    stack_frame_entry_t* frame_entry = nullptr;
    auto stack_frame = instruction_block->stack_frame();
    if (stack_frame != nullptr) {
        frame_entry = stack_frame->find_up(symbol_->name());
    }
    context.allocate_variable(r, symbol_->name(), type_, usage_, frame_entry);
    return true;
}

identifier_usage_t identifier::usage() const
{
    return usage_;
}

void identifier::usage(identifier_usage_t value)
{
    usage_ = value;
}

compiler::symbol_element *identifier::symbol() const
{
    return symbol_;
}

bool identifier::on_is_constant() const
{
    return symbol_->is_constant();
}

void identifier::initializer(compiler::initializer *value)
{
    initializer_ = value;
}

void identifier::on_owned_elements(element_list_t &list)
{
    if (initializer_ != nullptr) {
        list.emplace_back(initializer_);
    }

    if (symbol_ != nullptr) {
        list.emplace_back(symbol_);
    }
}

compiler::type *identifier::on_infer_type(const compiler::program *program)
{
    return type_;
}
}