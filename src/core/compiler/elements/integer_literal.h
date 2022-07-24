//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#define COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class integer_literal : public element {
public:
	explicit integer_literal(element* parent, uint64_t value);

	uint64_t value() const;

protected:
    bool on_is_constant() const override;

    bool on_as_integer(uint64_t &value) const override;

    compiler::type* on_infer_type(const compiler::program* program) override;

    bool on_emit(result &r, assembler &assembler, const emit_context_t& context) override;

private:
	uint64_t value_;
};
}

#endif // COMPILER_ELEMENTS_INTEGER_LITERAL_H_
