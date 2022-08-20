//
// Created by 12132 on 2022/7/17.
//

#include "type_info.h"
#include "../program.h"
namespace gfx::compiler {

type_info::type_info(block *parent_scope, block* scope)
    : compiler::composite_type(parent_scope, composite_types_t::struct_type, scope, nullptr, element_type_t::type_info)
{

}

bool type_info::on_initialize(result &r, compiler::program *program)
{
    symbol(program->make_symbol(parent_scope(), "type"));
    auto block_scope = parent_scope();
    auto string_type = program->find_type(qualified_symbol_t{.name = "string"});
    auto name_identifier = program->make_identifier(block_scope,
        program->make_symbol(block_scope,  "name"), nullptr, true);
    name_identifier->type(string_type);
    auto name_field = program->make_field(block_scope, name_identifier);
    fields().add(name_field);
    return composite_type::on_initialize(r, program);
}
}