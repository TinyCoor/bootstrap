//
// Created by 12132 on 2022/11/4.
//

#include "type_reference.h"
namespace gfx::compiler {

type_reference::type_reference(compiler::module *module, block *parent_scope,
                               const qualified_symbol_t &symbol, compiler::type *type)
    : element(module, parent_scope, element_type_t::type_reference),
      symbol_(symbol), type_(type)
{

}
bool type_reference::resolved() const
{
    return type_!=nullptr;
}
compiler::type *type_reference::type()
{
    return type_;
}
void type_reference::type(compiler::type *value)
{
    type_ = value;
}
const qualified_symbol_t &type_reference::symbol() const
{
    return symbol_;
}
bool type_reference::on_is_constant() const
{
    return true;
}

bool type_reference::on_infer_type(const session &session, type_inference_result_t& result)
{
    result.type = type_;
    return true;
}
}