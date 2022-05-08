//
// Created by 12132 on 2022/5/3.
//

#include "label.h"
namespace gfx::compiler {
label::label(element* parent, const std::string& name)
	: element(parent, element_type_t::label),name_(name)
{

}

std::string label::name() const
{
	return name_;
}
}