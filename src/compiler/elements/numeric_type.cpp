//
// Created by 12132 on 2022/5/2.
//

#include "numeric_type.h"
namespace gfx::compiler {
numeric_type::numeric_type(element* parent, const std::string &name, int64_t min, uint64_t max)
	: type(parent,name), min_(min), max_(max)
{

}

}