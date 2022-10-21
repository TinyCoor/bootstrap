//
// Created by 12132 on 2022/10/14.
//

#include "transmute.h"
#include <vm/instruction_block.h>
#include "types/type.h"
#include "symbol_element.h"
#include "fmt/format.h"
#include "core/compiler/session.h"
namespace gfx::compiler {
transmute::transmute(compiler::module* module, block* parent_scope, compiler::type* type, element* expr)
    : element(module, parent_scope, element_type_t::cast), expression_(expr), type_(type)
{
}

bool transmute::on_emit(compiler::session &session)
{
    if (expression_ == nullptr) {
        return true;
    }

    auto instruction_block = session.assembler().current_block();
    instruction_block->current_entry()->comment(
        fmt::format("XXX: transmute<{}> not yet implemented", type_->symbol()->name()),
        session.emit_context().indent);
    return expression_->emit(session);
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

compiler::type* transmute::on_infer_type(const compiler::session& session)
{
    return type_;
}
}