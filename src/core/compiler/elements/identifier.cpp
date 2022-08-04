//
// Created by 12132 on 2022/4/23.
//

#include "identifier.h"
#include "type.h"
#include "initializer.h"
#include "fmt/format.h"
#include "vm/instruction_block.h"
namespace gfx::compiler {
identifier::identifier(block *parent, const std::string& name, compiler::initializer* initializer)
	: element(parent, element_type_t::identifier), name_(name), initializer_(initializer) {
}

compiler::type* identifier::type()
{
	return type_;
}

std::string identifier::name() const
{
	return name_;
}

bool identifier::constant() const
{
	return constant_;
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

void identifier::constant(bool value)
{
	constant_ =value;
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
    auto instruction_block = context.assembler->current_block();
    if (type_->element_type() == element_type_t::namespace_type) {
        return true;
    }
    auto assembler = context.assembler;
    auto target_reg = instruction_block->current_target_register();
    if (target_reg == nullptr) {
        return true;
    }

    if (context.current_access() == emit_access_type_t::write) {
        if (assembler->in_procedure_scope() && usage_ == identifier_usage_t::stack) {
            emit_stack_based_load(instruction_block);
        } else {
            instruction_block->move_label_to_ireg(target_reg->reg.i, name_);
        }
    } else {
        switch (type_->element_type()) {
            case element_type_t::bool_type:
            case element_type_t::numeric_type: {
                if (assembler->in_procedure_scope() && usage_ == identifier_usage_t::stack) {
                    emit_stack_based_load(instruction_block);
                } else {
                    i_registers_t ptr_reg;
                    if (!instruction_block->allocate_reg(ptr_reg)) {

                    }
                    instruction_block->move_label_to_ireg(ptr_reg, name_);
                    instruction_block->load_to_ireg<uint64_t>(target_reg->reg.i, ptr_reg);
                    instruction_block->free_reg(ptr_reg);
                }
                break;
            }
            default: {
                instruction_block->move_label_to_ireg(target_reg->reg.i, name_);
                break;
            }
        }
    }

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

bool identifier::on_is_constant() const
{
    return constant_;
}

void identifier::emit_stack_based_load(instruction_block* instruction_block)
{
    auto target_reg = instruction_block->current_target_register();
    auto entry = instruction_block->stack_frame()->find_up(name());
    if (entry == nullptr) {
        // XXX: error
        return;
    }
    instruction_block->comment(fmt::format("{} identifier: {}", stack_frame_entry_type_name(entry->type),
        name()));
    instruction_block->load_to_ireg<uint64_t>(target_reg->reg.i, i_registers_t::fp, entry->offset);
}
}