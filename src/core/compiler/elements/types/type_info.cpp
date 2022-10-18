//
// Created by 12132 on 2022/7/17.
//

#include "type_info.h"
#include "core/compiler/session.h"
namespace gfx::compiler {

type_info::type_info(compiler::module* module, block *parent_scope, block* scope)
    : compiler::composite_type(module, parent_scope, composite_types_t::struct_type,
                               scope, nullptr, element_type_t::type_info)
{

}

bool type_info::on_initialize(compiler::session& session)
{
    auto program = &session.program();
    auto &builder = program->builder();
    symbol(builder.make_symbol(parent_scope(), "type"));
    auto block_scope = parent_scope();
    auto string_type = program->find_type(qualified_symbol_t{.name = "string"});
    auto name_identifier = builder.make_identifier(block_scope,
        builder.make_symbol(block_scope,  "name"), nullptr);
    name_identifier->type(string_type);
    auto name_field = builder.make_field(block_scope, name_identifier);
    fields().add(name_field);
    return composite_type::on_initialize(session);
}
type_access_model_t type_info::on_access_model() const
{
    return type_access_model_t::pointer;
}
}