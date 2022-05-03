//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#define COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#include "type.h"

namespace gfx::compiler {
class boolean_literal : public type{
public:
	boolean_literal(element* parent, const std::string& name, bool value);

	bool value() const
	{
		return value_;
	}

private:
	bool value_ = false;
};
}

#endif // COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
