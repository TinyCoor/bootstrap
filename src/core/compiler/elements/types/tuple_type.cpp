//
// Created by 12132 on 2022/8/4.
//

#include "tuple_type.h"
#include "../../session.h"
#include "../program.h"
namespace gfx::compiler {

tuple_type::tuple_type(compiler::module* module, block *parent_scope, block* scope)
    : composite_type(module, parent_scope, composite_types_t::struct_type, scope, nullptr, element_type_t::tuple_type)
{

}

bool tuple_type::on_initialize(compiler::session& session)
{
    symbol(session.builder().make_symbol(parent_scope(), "tuple"));
    return true;
}

type_access_model_t tuple_type::on_access_model() const
{
    return type_access_model_t::pointer;
}

}