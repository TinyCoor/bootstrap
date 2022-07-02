//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INITIALIZER_H_
#define COMPILER_ELEMENTS_INITIALIZER_H_
#include "element.h"

namespace gfx::compiler {
class initializer : public element {
public:
	explicit initializer(element* parent, element* expr);

	element* expression();

	compiler::procedure_type* procedure_type();

private:
	element* expr_ = nullptr;
};
}

#endif //COMPILER_ELEMENTS_INITIALIZER_H_
