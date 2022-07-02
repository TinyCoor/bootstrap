//
// Created by 12132 on 2022/5/3.
//

#include "composite_type.h"
namespace gfx::compiler {
composite_type::composite_type(element* parent,composite_types_t type, const std::string& name)
	: compiler::type(parent, element_type_t::composite_type, name),type_(type)
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
}