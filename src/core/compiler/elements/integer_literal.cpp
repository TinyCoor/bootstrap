//
// Created by 12132 on 2022/4/23.
//

#include "integer_literal.h"
#include "program.h"
namespace gfx::compiler {
integer_literal::integer_literal(element* parent, uint64_t value)
	: element(parent, element_type_t::integer_literal),value_(value)
{
}

uint64_t integer_literal::value() const
{
	return value_;
}

compiler::type *integer_literal::on_infer_type(const compiler::program *program)
{
	// XXX: i'm a bad person, i should do type narrowing here
	return program->find_type("u32");
}

bool integer_literal::on_as_integer(uint64_t &value) const
{
    value = value_;
    return true;
}

bool compiler::integer_literal::on_emit(gfx::result &r, gfx::assembler &assembler)
{
    auto instruction_block = assembler.current_block();
    auto dest_reg = instruction_block->allocate_ireg();
    instruction_block->move_u32_to_ireg(dest_reg, static_cast<uint32_t>(value_));
    instruction_block->free_ireg(dest_reg);
    return true;
}

}