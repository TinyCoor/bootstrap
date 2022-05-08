//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#define COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#include "type.h"

namespace gfx::compiler {
class boolean_literal : public element {
public:
	boolean_literal(element* parent, bool value);

	bool value() const;

private:
	bool value_ = false;
};
}

#endif // COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
