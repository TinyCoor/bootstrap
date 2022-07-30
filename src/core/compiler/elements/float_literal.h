//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FLOAT_LITERAL_H_
#define COMPILER_ELEMENTS_FLOAT_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class float_literal : public element {
public:
	explicit float_literal(element* parent, double value);

	double value() const;

protected:
    bool on_as_float(double &value) const override;

    compiler::type* on_infer_type(const compiler::program* program) override;
    bool on_emit(result &r, assembler &assembler, emit_context_t &context) override;
    bool on_is_constant() const override;

private:
	double value_;
};

}

#endif // COMPILER_ELEMENTS_FLOAT_LITERAL_H_
