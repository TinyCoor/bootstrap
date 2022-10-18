//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_EXPRESSION_H_
#define COMPILER_ELEMENTS_EXPRESSION_H_

#include "element.h"
#include "element_types.h"

namespace gfx::compiler {
class expression : public element {
public:
	explicit expression(compiler::module* module, block *parent_scope, element* root);

	element* root();

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session& session) override;

    void on_owned_elements(element_list_t& list) override;

	compiler::type* on_infer_type(const compiler::program* program) override;

private:
	element* root_;
};
}

#endif // COMPILER_ELEMENTS_EXPRESSION_H_
