//
// Created by 12132 on 2022/4/23.
//

#include "identifier.h"
#include "type.h"
#include "initializer.h"
namespace gfx::compiler {
identifier::identifier(element *parent, const std::string& name, compiler::initializer* initializer)
	: element(parent,element_type_t::identifier), name_(name), initializer_(initializer) {
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

bool identifier::on_emit(result &r, assembler &assembler, const emit_context_t &context)
{
    if (type_->element_type() == element_type_t::namespace_type) {
        return true;
    }

    auto instruction_block = assembler.current_block();
    auto target_reg = instruction_block->current_target_register();
    if (target_reg == nullptr) {
        return true;
    }
    instruction_block->move_label_to_ireg(target_reg->reg.i, name_);
    return true;
}

}