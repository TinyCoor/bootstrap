//
// Created by 12132 on 2022/4/23.
//

#include "type.h"
#include "field.h"
namespace gfx::compiler {
type::type(element* parent, element_type_t type, const std::string& name)
	: element(parent,type), name_(name)
{
}

type::~type()
{
}
std::string type::name() const
{
	return name_;
}

void type::name(const std::string &value)
{
	name_ =value;
}

size_t type::size_in_bytes() const
{
	return size_in_bytes_;
}

bool type::initialize(result &r)
{
	return on_initialize(r);
}

void type::size_in_bytes(size_t value)
{
	size_in_bytes_ = value;
}

bool type::on_initialize(result &r)
{
	return true;
}

}