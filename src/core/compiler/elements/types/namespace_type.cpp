//
// Created by 12132 on 2022/5/22.
//

#include "namespace_type.h"
#include "../program.h"
#include "../symbol_element.h"
namespace gfx::compiler{
namespace_type::namespace_type(block *parent)
	: type(parent, element_type_t::namespace_type, nullptr)
{

}

bool namespace_type::on_initialize(result &r, compiler::program* program)
{
    auto &builder = program->builder();
    symbol(builder.make_symbol(parent_scope(), "namespace"));
	return true;
}
bool namespace_type::on_type_check(compiler::type *other)
{
    return other != nullptr
        && other->symbol()->name() == symbol()->name();
}
}