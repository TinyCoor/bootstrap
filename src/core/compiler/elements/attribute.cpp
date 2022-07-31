//
// Created by 12132 on 2022/4/23.
//

#include "attribute.h"

namespace gfx::compiler {
attribute::attribute(block* parent, const std::string& name, element* expr)
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

bool attribute::on_as_string(std::string& value) const
{
    if (expr_ == nullptr) {
        return false;
    }
    return expr_->as_string(value);
}

bool attribute::on_as_bool(bool &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_bool(value);
}

bool attribute::on_as_float(double &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_float(value);
}

bool attribute::on_as_integer(uint64_t &value) const
{
    if (!expr_) {
        return false;
    }
    return expr_->as_integer(value);
}
}