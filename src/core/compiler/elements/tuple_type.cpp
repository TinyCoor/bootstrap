//
// Created by 12132 on 2022/8/4.
//

#include "tuple_type.h"
namespace gfx::compiler {

tuple_type::tuple_type(block *parent_scope)
    : composite_type(parent_scope, composite_types_t::struct_type, "tuple")
{

}

bool tuple_type::on_initialize(result &r, compiler::program *program)
{
    return true;
}

}