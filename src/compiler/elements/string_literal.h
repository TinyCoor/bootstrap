//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_STRING_LITERAL_H_
#define COMPILER_ELEMENTS_STRING_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class string_literal : public element {
public:
	explicit string_literal(element* parent, const std::string& value);

	inline std::string value() const
	{
		return value_;
	}

private:
	std::string value_;
};
}

#endif // COMPILER_ELEMENTS_STRING_LITERAL_H_
