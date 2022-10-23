//
// Created by 12132 on 2022/5/2.
//

#include "boolean_literal.h"
#include "core/compiler/session.h"
#include "program.h"
namespace gfx::compiler{
boolean_literal::boolean_literal(compiler::module* module,block* parent, bool value)
	: element(module, parent, element_type_t::boolean_literal), value_(value)
{

}

bool boolean_literal::value() const
{
	return value_;
}

bool boolean_literal::on_infer_type(const compiler::session& session, type_inference_result_t& result)
{
	result.type =  session.scope_manager().find_type(qualified_symbol_t{.name = "bool"});
    return result.type != nullptr;
}

bool boolean_literal::on_as_bool(bool &value) const
{
    value = value_;
    return true;
}
bool boolean_literal::on_emit(compiler::session &session)
{
    auto instruction_block = session.assembler().current_block();
    auto target_reg = session.assembler().current_target_register();
    instruction_block->move_constant_to_reg(*target_reg, static_cast<uint64_t>(value_ ? 1 : 0));
    return true;
}

bool boolean_literal::on_is_constant() const
{
    return true;
}
}