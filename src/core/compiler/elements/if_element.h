//
// Created by 12132 on 2022/5/14.
//

#ifndef COMPILER_ELEMENTS_IF_ELEMENT_H_
#define COMPILER_ELEMENTS_IF_ELEMENT_H_
#include "element.h"
namespace gfx::compiler {
class if_element : public element {
public:
	if_element(compiler::module* module, block* parent_scope, element* predicate, element* true_branch, element* false_branch);

	element* predicate();

	element* true_branch();

	element* false_branch();
protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

private:
	element* predicate_ = nullptr;
	element* true_branch_ = nullptr;
	element* false_branch_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_IF_ELEMENT_H_
