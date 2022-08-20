//
// Created by 12132 on 2022/8/4.
//

#include "tuple_type.h"
#include "../program.h"
namespace gfx::compiler {

tuple_type::tuple_type(block *parent_scope, block* scope)
    : composite_type(parent_scope, composite_types_t::struct_type, scope, nullptr)
{

}

bool tuple_type::on_initialize(result &r, compiler::program *program)
{
    symbol(program->make_symbol(parent_scope(), "tuple"));
    return true;
}

}