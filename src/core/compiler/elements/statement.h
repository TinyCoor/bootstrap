//
// Created by 12132 on 2022/5/8.
//

#ifndef COMPILER_ELEMENTS_STATEMENT_H_
#define COMPILER_ELEMENTS_STATEMENT_H_
#include "element.h"

namespace gfx::compiler {
class statement : public element {
public:
	statement(compiler::module* module, block* parent_scope, element* expr);

	element* expression();

	label_list_t& labels();

protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

private:
	label_list_t labels_ {};
	compiler::element* expression_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_STATEMENT_H_
