//
// Created by 12132 on 2022/4/23.
//

#include "attribute.h"
#include "float_literal.h"
#include "string_literal.h"
#include "integer_literal.h"
#include "boolean_literal.h"
namespace gfx::compiler {
attribute::attribute(element* parent, const std::string& name, element* expr)
	: element(parent, element_type_t::attribute), name_(name), expr_(expr)
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

std::string attribute::as_string() const
{
	switch (expr_->element_type()) {
		case element_type_t::float_literal:
			return std::to_string(dynamic_cast<float_literal*>(expr_)->value());
		case element_type_t::string_literal:
			return dynamic_cast<string_literal*>(expr_)->value();
		case element_type_t::boolean_literal:
			return std::to_string(dynamic_cast<boolean_literal*>(expr_)->value());
		case element_type_t::integer_literal:
			return std::to_string(dynamic_cast<integer_literal*>(expr_)->value());
		default:
			break;
	}
	return "";
}
}