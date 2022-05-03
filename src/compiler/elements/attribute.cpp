//
// Created by 12132 on 2022/4/23.
//

#include "attribute.h"
namespace gfx::compiler {
attribute::attribute(element* parent, const std::string& name, element* expr)
	: element(parent), name_(name), expr_(expr)
{}

attribute::~attribute() noexcept
{
}
element *attribute::expression()
{
	return expr_;
}

std::string attribute::name() const
{
	return name_;
}
}