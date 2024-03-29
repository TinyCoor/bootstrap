//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_BINARY_OPERATOR_H_
#define COMPILER_ELEMENTS_BINARY_OPERATOR_H_
#include "operator_base.h"
#include "element_types.h"
namespace gfx::compiler {

class binary_operator : public operator_base {
public:
	binary_operator(compiler::module* module, block* parent_scope, operator_type_t type,
                    element* lhs, element* rhs);

	element* lhs();

	element* rhs();

    void lhs(compiler::element* element);

    void rhs(compiler::element* element);

protected:
    bool on_is_constant() const override;

    void on_owned_elements(element_list_t& list) override;

    bool on_emit(compiler::session& session) override;

    bool on_fold(compiler::session& session, fold_result_t& result) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t &result) override;

    void emit_relational_operator(compiler::session& session, instruction_block* instruction_block);

    void emit_arithmetic_operator(compiler::session& session, instruction_block* instruction_block);
private:
	element* lhs_ = nullptr;
	element* rhs_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_BINARY_OPERATOR_H_
