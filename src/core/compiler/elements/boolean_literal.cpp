//
// Created by 12132 on 2022/5/2.
//

#include "boolean_literal.h"
#include "program.h"
namespace gfx::compiler{
boolean_literal::boolean_literal(block* parent, bool value)
	: element(parent, element_type_t::boolean_literal), value_(value)
{

}

bool boolean_literal::value() const
{
	return value_;
}

compiler::type *boolean_literal::on_infer_type(const compiler::program *program)
{
	return program->find_type("bool");
}

bool boolean_literal::on_as_bool(bool &value) const
{
    value = value_;
    return true;
}
bool boolean_literal::on_emit(result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->current_block();
    auto target_reg = instruction_block->current_target_register();
    instruction_block->move_to_ireg(target_reg->reg.i,
        static_cast<uint8_t>(value_ ? 1 : 0));
    return true;
}

bool boolean_literal::on_is_constant() const
{
    return true;
}
}