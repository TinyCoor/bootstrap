//
// Created by 12132 on 2022/7/17.
//

#include "type_info.h"
#include "program.h"
namespace gfx::compiler {

type_info::type_info(element *parent)
    : compiler::composite_type(parent, composite_types_t::struct_type, "type", element_type_t::type_info)
{

}

bool type_info::on_initialize(result &r, compiler::program *program)
{
    auto block_scope = dynamic_cast<compiler::block*>(parent());
    auto string_type = program->find_type_down("string");
    auto name_identifier = program->make_identifier(block_scope, "name", nullptr);
    name_identifier->type(string_type);
    auto name_field = program->make_field(block_scope, name_identifier);
    fields().add(name_field);
    return composite_type::on_initialize(r, program);
}
}