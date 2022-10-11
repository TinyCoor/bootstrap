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
    auto &builder = program->builder();
    symbol(builder.make_symbol(parent_scope(), "bool"));
    size_in_bytes(1);
    return true;
}

type_number_class_t bool_type::on_number_class() const
{
    return type_number_class_t::integer;
}

type_access_model_t bool_type::on_access_model() const
{
    return type_access_model_t::value;
}

bool bool_type::on_type_check(compiler::type *other)
{
    return other != nullptr
        && other->element_type() == element_type_t::bool_type;
}
}