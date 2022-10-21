//
// Created by 12132 on 2022/9/14.
//

#include "module_reference.h"
#include "core/compiler/session.h"
#include "program.h"
namespace gfx::compiler {
module_reference::module_reference(compiler::module* module, compiler::block *parent_scope, compiler::element *expr)
    : element(module, parent_scope, element_type_t::module_reference), expression_(expr)
{

}
compiler::element *module_reference::expression()
{
    return expression_;
}

void module_reference::on_owned_elements(element_list_t &list)
{
    if (expression_ !=nullptr) {
        list.emplace_back(expression_);
    }
}

compiler::type *module_reference::on_infer_type(const compiler::session& session)
{
    return session.program().find_type({.name = "module"});
}
compiler::module *module_reference::reference()
{
    return module_;
}
void module_reference::reference(compiler::module *value)
{
    module_ = value;
}

bool module_reference::on_is_constant() const
{
    return true;
}

}