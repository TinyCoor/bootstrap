//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_FLOAT_LITERAL_H_
#define COMPILER_ELEMENTS_FLOAT_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class float_literal : public element {
public:
	explicit float_literal(compiler::module* module, block* parent_scope, double value);

	double value() const;

    bool is_signed() const;

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session &session) override;

    bool on_as_float(double &value) const override;

    compiler::type* on_infer_type(const compiler::session& session) override;

private:
	double value_;
};

}

#endif // COMPILER_ELEMENTS_FLOAT_LITERAL_H_
