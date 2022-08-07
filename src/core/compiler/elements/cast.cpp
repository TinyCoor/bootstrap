//
// Created by 12132 on 2022/5/8.
//

#include "cast.h"
#include "types/type.h"
#include "symbol_element.h"
#include "fmt/format.h"
namespace gfx::compiler {
cast::cast(block* parent, compiler::type* type, element* expr)
	: element(parent, element_type_t::cast), expression_(expr), type_(type)
{
}

element* cast::expression()
{
	return expression_;
}

compiler::type* cast::type()
{
	return type_;
}

compiler::type *cast::on_infer_type(const compiler::program *program)
{
	return type_;
}

bool cast::on_emit(result &r, emit_context_t &context)
{
    if (expression_ == nullptr) {
       return true;
    }
    auto assembler = context.assembler;
    auto instruction_block = assembler->current_block();
    instruction_block->current_entry()->comment(fmt::format("XXX: cast<{}> not yet implemented", type_->symbol()->name()));
    return expression_->emit(r, context);
}

}