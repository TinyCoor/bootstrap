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

std::string symbol_element::fully_qualified_name() const
{
    std::stringstream stream {};
    auto count = 0;
    for (const auto& name : namespaces_) {
        stream << name;
        if (count > 0) {
            stream << "::";
        }
        count++;
    }
    stream << name_;
    return stream.str();
}

}
