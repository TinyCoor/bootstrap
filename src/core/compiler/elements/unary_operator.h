//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_UNARY_OPERATOR_H_
#define COMPILER_ELEMENTS_UNARY_OPERATOR_H_
#include "operator_base.h"
#include "element_types.h"

namespace gfx::compiler {
class unary_operator : public operator_base {
public:
	unary_operator(compiler::module* module, block* parent_scope, operator_type_t type, element* rhs);

	element* rhs();

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

    element* on_fold(compiler::session& session) override;

    compiler::type* on_infer_type(const compiler::session& session) override;

private:
	element* rhs_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_UNARY_OPERATOR_H_
