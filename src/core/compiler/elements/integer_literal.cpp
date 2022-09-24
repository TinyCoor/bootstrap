//
// Created by 12132 on 2022/4/23.
//

#include "integer_literal.h"
#include "common/bytes.h"
#include "types/numeric_type.h"
#include "program.h"
namespace gfx::compiler {
integer_literal::integer_literal(block* parent, uint64_t value)
	: element(parent, element_type_t::integer_literal),value_(value)
{
}

uint64_t integer_literal::value() const
{
	return value_;
}

compiler::type *integer_literal::on_infer_type(const compiler::program *program)
{
	/// XXX: i'm a bad person, i should do type narrowing here
	return program->find_type(qualified_symbol_t{.name = numeric_type::narrow_to_value(value_)});
}

bool integer_literal::on_as_integer(uint64_t &value) const
{
    value = value_;
    return true;
}

bool compiler::integer_literal::on_emit(gfx::result &r, emit_context_t& context)
{
    auto assembler = context.assembler;
    auto instruction_block = assembler->current_block();
    auto target_reg = assembler->current_target_register();
    auto inferred_type = infer_type(context.program);
    instruction_block->move_constant_to_ireg(op_size_for_byte_size(inferred_type->size_in_bytes()),
        target_reg->reg.i, value_);
    return true;
}

bool integer_literal::on_is_constant() const
{
    return true;
}

bool integer_literal::is_signed() const
{
    return is_sign_bit_set(value_);
}

}