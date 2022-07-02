//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_ATTRIBUTE_H_
#define COMPILER_ELEMENTS_ATTRIBUTE_H_
#include <string>
#include "element.h"

namespace gfx::compiler {
class attribute : public element {
public:
	attribute(element* parent, const std::string& name, element* expr);

	element* expression();

	std::string name() const;

	std::string as_string() const;

private:
	std::string name_;
	element* expr_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_ATTRIBUTE_H_
