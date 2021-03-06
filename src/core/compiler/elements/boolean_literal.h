//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#define COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
#include "element.h"

namespace gfx::compiler {

class boolean_literal : public element {
public:
	boolean_literal(element* parent, bool value);

	bool value() const;

protected:
    bool on_is_constant() const override;

    bool on_as_bool(bool &value) const override;

	compiler::type* on_infer_type(const compiler::program* program) override;

    bool on_emit(result &r, assembler &assembler, const emit_context_t &context) override;

private:
	bool value_ = false;
};
}

#endif // COMPILER_ELEMENTS_BOOLEAN_LITERAL_H_
