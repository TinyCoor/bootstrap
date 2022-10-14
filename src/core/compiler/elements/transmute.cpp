//
// Created by 12132 on 2022/10/14.
//

#include "transmute.h"
#include <vm/instruction_block.h>
#include "types/type.h"
#include "symbol_element.h"
#include "fmt/format.h"
namespace gfx::compiler {
transmute::transmute(block* parent_scope, compiler::type* type, element* expr)
    : element(parent_scope, element_type_t::cast), expression_(expr), type_(type)
{
}

bool transmute::on_emit(result& r, emit_context_t& context)
{
    if (expression_ == nullptr) {
        return true;
    }

    auto instruction_block = context.assembler->current_block();
    instruction_block->current_entry()->comment(
        fmt::format("XXX: transmute<{}> not yet implemented", type_->symbol()->name()),
        context.indent);
    return expression_->emit(r, context);
}

element* transmute::expression()
{
    return expression_;
}

compiler::type* transmute::type()
{
    return type_;
}

void transmute::on_owned_elements(element_list_t& list)
{
    if (expression_ != nullptr) {
        list.emplace_back(expression_);
    }
}

compiler::type* transmute::on_infer_type(const compiler::program* program)
{
    return type_;
}
}