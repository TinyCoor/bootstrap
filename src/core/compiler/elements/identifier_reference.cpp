//
// Created by 12132 on 2022/8/20.
//

#include "identifier_reference.h"
#include "identifier.h"
namespace gfx::compiler {
identifier_reference::identifier_reference(block* parent_scope,
    const qualified_symbol_t& symbol, compiler::identifier* identifier)
        : element(parent_scope, element_type_t::identifier_reference),
        symbol_(symbol), identifier_(identifier) {
}

bool identifier_reference::resolved() const
{
    return identifier_ != nullptr;
}

compiler::identifier* identifier_reference::identifier()
{
    return identifier_;
}

const qualified_symbol_t& identifier_reference::symbol() const
{
    return symbol_;
}

void identifier_reference::identifier(compiler::identifier* value)
{
    identifier_ = value;
}

bool identifier_reference::on_emit(result &r, emit_context_t &context)
{
    if (identifier_ == nullptr) {
        return false;
    }
    return identifier_->emit(r, context);
}

bool identifier_reference::on_is_constant() const
{
    if (identifier_ == nullptr) {
        return false;
    }
    return identifier_->is_constant();
}

}