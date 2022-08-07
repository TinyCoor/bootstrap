//
// Created by 12132 on 2022/7/17.
//

#include "struct_type.h"
namespace gfx::compiler {
struct_type::struct_type(block *parent, symbol_element* symbol)
    : composite_type(parent, composite_types_t::struct_type, symbol, element_type_t::struct_type)
{

}

bool struct_type::on_initialize(result &r, compiler::program *program)
{
    return true;
}
}
