//
// Created by 12132 on 2022/5/8.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_STATEMENT_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_STATEMENT_H_
#include "element.h"

namespace gfx::compiler {
class statement : public element {
public:
	statement(element* parent, element* expr);

	element* expr();

	label_list_t& labels();

private:
	label_list_t labels_ {};
	compiler::element* expr_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_STATEMENT_H_
