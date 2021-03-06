//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FIELD_H_
#define COMPILER_ELEMENTS_FIELD_H_

#include "element.h"
#include "initializer.h"
namespace gfx::compiler {
class field : public element {
public:
	field(element* parent,  compiler::identifier* identifier);

	compiler::identifier* identifier();

private:
	compiler::identifier* identifier_;
};

}

#endif // COMPILER_ELEMENTS_FIELD_H_
