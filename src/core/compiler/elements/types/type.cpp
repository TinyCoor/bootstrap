//
// Created by 12132 on 2022/4/23.
//

#include "type.h"
#include "../symbol_element.h"
namespace gfx::compiler {
type::type(block* parent, element_type_t type, compiler::symbol_element* symbol)
	: element(parent,type), symbol_(symbol)
{
}

type::~type()
{
}

size_t type::size_in_bytes() const
{
	return size_in_bytes_;
}

bool type::initialize(result &r,  compiler::program* program)
{
	return on_initialize(r, program);
}

void type::size_in_bytes(size_t value)
{
	size_in_bytes_ = value;
}

bool type::on_initialize(result &r, compiler::program* program)
{
	return true;
}

bool type::packed() const
{
    return packed_;
}

void type::packed(bool value)
{
    packed_ = value;
}

size_t type::alignment() const
{
    return alignment_;
}
void type::alignment(size_t value)
{
    alignment_ = value;
}

compiler::symbol_element *type::symbol() const
{
    return symbol_;
}

void type::symbol(compiler::symbol_element *value)
{
    symbol_ = value;
}
void type::on_owned_elements(element_list_t &list)
{
    if (symbol_ != nullptr) {
        list.emplace_back(symbol_);
    }
}

}