//
// Created by 12132 on 2022/5/3.
//

#include "composite_type.h"
namespace gfx::compiler {
composite_type::composite_type(element* parent,composite_types_t type, const std::string name)
	: compiler::type(parent, name),type_(type)
{

}
}