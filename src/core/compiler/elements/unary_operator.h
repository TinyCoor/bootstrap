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
	unary_operator(block* parent_scope, operator_type_t type, element* rhs);

	element* rhs();

protected:
	compiler::type* on_infer_type(const compiler::program* program) override;
    bool on_is_constant() const override;
    bool on_emit(result &r, emit_context_t& context) override;
private:
	element* rhs_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_UNARY_OPERATOR_H_
