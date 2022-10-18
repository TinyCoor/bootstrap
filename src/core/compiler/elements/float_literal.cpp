//
// Created by 12132 on 2022/4/23.
//

#include "float_literal.h"
#include "types/numeric_type.h"
#include "program.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
float_literal::float_literal(compiler::module* module, block* parent,double value)
	: element(module, parent, element_type_t::float_literal), value_(value)
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

bool compiler::float_literal::on_emit(compiler::session &session)
{
    auto instruction_block = session.assembler().current_block();
    auto target_reg = session.assembler().current_target_register();
    instruction_block->move_constant_to_reg(*target_reg, value_);
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