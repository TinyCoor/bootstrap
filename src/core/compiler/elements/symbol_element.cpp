//
// Created by 12132 on 2022/8/6.
//

#include "symbol_element.h"
namespace gfx::compiler {
symbol_element::symbol_element(compiler::block *parent_scope, const std::string &name,
    const string_list_t &namespaces)
    : element(parent_scope, element_type_t::symbol),
      name_(name), namespaces_(namespaces)
{
}

void symbol_element::constant(bool value)
{
    is_constant_ = value;
}

bool symbol_element::on_is_constant() const
{
    return is_constant_;
}

const string_list_t &symbol_element::namespaces() const
{
    return namespaces_;
}

std::string symbol_element::name() const
{
    return name_;
}

bool symbol_element::is_qualified() const
{
    return !namespaces_.empty();
}

std::string symbol_element::fully_qualified_name()
{
    if (fully_qualified_name_.empty()) {
        cache_fully_qualified_name();
    }
    return fully_qualified_name_;
}

void symbol_element::cache_fully_qualified_name()
{
    fully_qualified_name_ = make_fully_qualified_name(this);
}

bool symbol_element::operator==(const symbol_element &other) const
{
    return fully_qualified_name_ == other.fully_qualified_name_;
}

bool symbol_element::operator==(const qualified_symbol_t &other) const
{
    return fully_qualified_name_ == other.fully_qualified_name;
}

qualified_symbol_t symbol_element::qualified_symbol() const
{
    qualified_symbol_t symbol {};
    symbol.name = name_;
    symbol.location = location();
    symbol.namespaces = namespaces_;
    symbol.fully_qualified_name = fully_qualified_name_;
    return symbol;
}

}
