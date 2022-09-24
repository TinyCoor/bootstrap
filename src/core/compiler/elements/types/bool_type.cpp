//
// Created by 12132 on 2022/9/24.
//

#include "bool_type.h"
#include "../program.h"
namespace gfx::compiler {
bool_type::bool_type(block *parent_scope)
    : compiler::type(parent_scope, element_type_t::bool_type, nullptr)
{

}

bool bool_type::on_initialize(result &r, compiler::program *program)
{
    return type::on_initialize(r, program);
}
}