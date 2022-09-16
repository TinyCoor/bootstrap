//
// Created by 12132 on 2022/9/14.
//

#include "module_type.h"
#include "../program.h"
namespace gfx::compiler {
module_type::module_type(block *parent_scope, block* scope)
    : composite_type(parent_scope, composite_types_t::struct_type, scope, nullptr, element_type_t::module_type)
{

}

bool module_type::on_initialize(result &r, compiler::program *program)
{
    symbol(program->make_symbol(parent_scope(), "module"));
    return true;
}
}