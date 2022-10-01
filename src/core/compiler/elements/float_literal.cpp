//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
#include "types/numeric_type.h"
#include "program.h"
namespace gfx::compiler {
float_literal::float_literal(block* parent,double value)
	: element(parent, element_type_t::float_literal), value_(value)
{
}

double float_literal::value() const
{
	return value_;
}

compiler::type *float_literal::on_infer_type(const compiler::program *program)
{
    return program->find_type({.name = numeric_type::narrow_to_value(value_)
                              });
}

bool float_literal::on_as_float(double &value) const
{
    value = value_;
    return true;
}

bool compiler::float_literal::on_emit(gfx::result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->current_block();
    auto target_reg = context.assembler->current_target_register();
    auto inferred_type = infer_type(context.program);
    instruction_block->move_constant_to_ireg(op_size_for_byte_size(inferred_type->size_in_bytes()),
         target_reg->i, value_);
    return true;
}

bool compiler::float_literal::on_is_constant() const
{
    return true;
}

bool float_literal::is_signed() const
{
    return value_ < 0;
}

}