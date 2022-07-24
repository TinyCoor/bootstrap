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
	binary_operator(element* parent, operator_type_t type, element* lhs, element* rhs);

	element* lhs();

	element* rhs();

protected:
    bool on_is_constant() const override;

	compiler::type* on_infer_type(const compiler::program* program) override;

    bool on_emit(result &r, assembler &assembler, const emit_context_t& context) override;

    void emit_relational_operator(result& r, assembler& assembler, const emit_context_t& context,
        instruction_block* instruction_block);

    void emit_arithmetic_operator(result& r, assembler& assembler, const emit_context_t& context,
        instruction_block* instruction_block);
private:
	element* lhs_ = nullptr;
	element* rhs_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_BINARY_OPERATOR_H_
