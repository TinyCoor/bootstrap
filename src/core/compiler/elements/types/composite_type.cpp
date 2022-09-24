//
// Created by 12132 on 2022/5/3.
//

#include "composite_type.h"
#include "core/compiler/elements/block.h"
#include "core/compiler/elements/identifier.h"
namespace gfx::compiler {
composite_type::composite_type(block* parent, composite_types_t type, block* scope,
                               compiler::symbol_element* symbol, element_type_t element_type)

	: compiler::type(parent, element_type, symbol), scope_(scope), type_(type)
{

}

field_map_t& composite_type::fields()
{
	return fields_;
}

type_map_t& composite_type::type_parameters()
{
	return type_parameters_;
}

composite_types_t composite_type::type() const
{
	return type_;
}

bool composite_type::on_initialize(result &r, compiler::program* program)
{
    size_t size = 0;
    for (auto &filed : fields_.as_list()) {
        size += filed->identifier()->type()->size_in_bytes();
    }
    size_in_bytes(size);
	return true;
}
block *composite_type::scope()
{
    return scope_;
}

bool composite_type::on_is_constant() const
{
    return true;
}
void composite_type::on_owned_elements(element_list_t &list)
{
    for (auto element : fields_.as_list()) {
        list.emplace_back(element);
    }

    if (scope_ != nullptr) {
        list.emplace_back(scope_);
    }
}

type_access_model_t composite_type::on_access_model() const
{
    return type_access_model_t::pointer;
}
}