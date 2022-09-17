//
// Created by 12132 on 2022/9/14.
//

#include "module_reference.h"
#include "program.h"
namespace gfx::compiler {
module_reference::module_reference(compiler::block *parent_scope, compiler::element *expr)
    : element(parent_scope, element_type_t::module_reference), expression_(expr)
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

compiler::type *module_reference::on_infer_type(const compiler::program *program)
{
    return program->find_type({.name = "module"});
}
compiler::module *module_reference::module()
{
    return module_;
}
void module_reference::module(compiler::module *value)
{
    module_ = value;
}

bool module_reference::on_is_constant() const
{
    return true;
}

}