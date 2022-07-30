//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
#include "program.h"
namespace gfx::compiler {
float_literal::float_literal(element* parent,double value)
	: element(parent, element_type_t::float_literal), value_(value)
{
}

double float_literal::value() const
{
	return value_;
}

compiler::type *float_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type("f64");
}

bool float_literal::on_as_float(double &value) const
{
    value = value_;
    return true;
}

bool compiler::float_literal::on_emit(gfx::result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->current_block();
    auto target_reg = instruction_block->current_target_register();
    instruction_block->move_to_ireg(target_reg->reg.i, static_cast<uint64_t>(value_));
    return true;
}

bool compiler::float_literal::on_is_constant() const
{
    return true;
}

}