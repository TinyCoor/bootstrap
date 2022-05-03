//
// Created by 12132 on 2022/5/2.
//

#include "array_type.h"
namespace gfx::compiler {
array_type::array_type(element* parent, const std::string& name, compiler::type* element_type)
	: type(parent, name), element_type_(element_type) {
}
}
