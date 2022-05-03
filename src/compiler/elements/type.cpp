//
// Created by 12132 on 2022/4/23.
//

#include "type.h"
#include "field.h"
namespace gfx::compiler {
type::type(element* parent, const std::string& name)
	: element(parent), name_(name)
{
}

type::~type()
{
}

}