//
// Created by 12132 on 2022/5/8.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_ALIAS_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_ALIAS_H_

#include "element.h"

namespace gfx::compiler {
class alias : public element {
public:
	alias(element* parent, element* expr);

	element* expression();
private:
	element* expression_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_ALIAS_H_
