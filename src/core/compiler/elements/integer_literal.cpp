//
// Created by 12132 on 2022/4/23.
//

#include "integer_literal.h"
#include "common/bytes.h"
#include "types/numeric_type.h"
#include "program.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
integer_literal::integer_literal(compiler::module* module, block* parent, uint64_t value)
	: element(module, parent, element_type_t::integer_literal),value_(value)
{
}

uint64_t integer_literal::value() const
{
	return value_;
}

bool integer_literal::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
	result.type =  session.scope_manager().find_type(qualified_symbol_t{.name = numeric_type::narrow_to_value(value_)});
    return result.type != nullptr;
}

bool integer_literal::on_as_integer(uint64_t &value) const
{
    value = value_;
    return true;
}

bool compiler::integer_literal::on_emit(compiler::session &session)
{
    auto &assembler = session.assembler();
    auto instruction_block = assembler.current_block();
    auto target_reg = assembler.current_target_register();
    instruction_block->move_constant_to_reg(*target_reg, value_);
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