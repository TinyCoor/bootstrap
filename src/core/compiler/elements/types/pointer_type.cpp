//
// Created by 12132 on 2022/9/17.
//

#include "pointer_type.h"
#include "fmt/format.h"
#include "../program.h"
#include "../symbol_element.h"
namespace gfx::compiler {
std::string pointer_type::name_for_pointer(compiler::type* base_type)
{
    return fmt::format("__ptr_{}__", base_type->symbol()->name());
}

pointer_type::pointer_type(compiler::block* parent_scope, compiler::type* base_type)
    : compiler::type(parent_scope, element_type_t::pointer_type, nullptr), base_type_(base_type)
{
}

bool pointer_type::on_initialize(result& r, compiler::program* program)
{
    auto type_symbol = program->make_symbol(parent_scope(), name_for_pointer(base_type_));
    symbol(type_symbol);
    type_symbol->parent_element(this);
    return true;
}

compiler::type* pointer_type::base_type() const
{
    return base_type_;
}
}