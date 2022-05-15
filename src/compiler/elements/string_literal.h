//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_STRING_LITERAL_H_
#define COMPILER_ELEMENTS_STRING_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class string_literal : public element {
public:
	string_literal(element* parent, const std::string& value);

	std::string value() const;

private:
	std::string value_;
};
}

#endif // COMPILER_ELEMENTS_STRING_LITERAL_H_
