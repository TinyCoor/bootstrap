//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#define COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class integer_literal : public element {
public:
    integer_literal(compiler::module* module, block* parent_scope, uint64_t value);

	uint64_t value() const;

    bool is_signed() const;

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session &session) override;

    bool on_as_integer(uint64_t &value) const override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
	uint64_t value_;
};
}

#endif // COMPILER_ELEMENTS_INTEGER_LITERAL_H_
