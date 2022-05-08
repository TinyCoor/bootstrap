//
// Created by 12132 on 2022/4/23.
//

#include "attribute.h"
namespace gfx::compiler {
attribute::attribute(element* parent, const std::string& name, element* expr)
	: element(parent, element_type_t::attribute), name_(name), expr_(expr)
{}

element *attribute::expression()
{
	return expr_;
}

std::string attribute::name() const
{
	return name_;
}
}