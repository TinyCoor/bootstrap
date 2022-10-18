//
// Created by 12132 on 2022/7/17.
//

#include "struct_type.h"
namespace gfx::compiler {
struct_type::struct_type(compiler::module* module, block *parent, symbol_element* symbol, block* scope)
    : composite_type(module, parent, composite_types_t::struct_type, scope, symbol,element_type_t::struct_type)
{

}

bool struct_type::on_initialize(compiler::session& session)
{
    return true;
}
}
