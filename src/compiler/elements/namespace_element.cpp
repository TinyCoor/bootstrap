//
// Created by 12132 on 2022/4/23.
//

#include "namespace_element.h"
namespace gfx::compiler {
namespace_element::namespace_element(block* parent, const std::string& name)
	: block(parent), name_(name) {
}

namespace_element::~namespace_element() {
}

std::string namespace_element::name() const {
	return name_;
}
}