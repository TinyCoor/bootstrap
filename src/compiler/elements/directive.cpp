//
// Created by 12132 on 2022/4/23.
//

#include "directive.h"
namespace gfx::compiler {
directive::directive(block* parent, const std::string& name, element* expression)
	: element(parent, element_type_t::directive),name_(name),expression_(expression)
{
}

std::string directive::name() const
{
	return name_;
}

element *directive::expression()
{
	return expression_;
}

}