//
// Created by 12132 on 2022/9/14.
//

#include "module_type.h"
#include "core/compiler/session.h"
#include "../symbol_element.h"
namespace gfx::compiler {
module_type::module_type(compiler::module* module, block *parent_scope, block* scope)
    : composite_type(module, parent_scope, composite_types_t::struct_type, scope, nullptr,
                     element_type_t::module_type)
{

}

bool module_type::on_initialize(compiler::session& session)
{
    symbol(session.builder().make_symbol(parent_scope(), "module"));
    return true;
}

bool module_type::on_type_check(compiler::type *other)
{
    return other != nullptr && other->symbol()->name() == symbol()->name();
}
}