//
// Created by 12132 on 2022/5/8.
//

#ifndef COMPILER_ELEMENTS_ALIAS_H_
#define COMPILER_ELEMENTS_ALIAS_H_

#include "element.h"

namespace gfx::compiler {
class alias : public element {
public:
	alias(block* parent_scope, element* expr);

	element* expression();

protected:
    void on_owned_elements(element_list_t& list) override;

private:
	element* expression_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ALIAS_H_
