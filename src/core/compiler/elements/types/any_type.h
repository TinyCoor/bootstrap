//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_ANY_TYPE_H_
#define COMPILER_ELEMENTS_ANY_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class any_type : public composite_type {
public:
	explicit any_type(block * parent_scope, block* scope);

    [[maybe_unused]] compiler::type* underlying_type();

    [[maybe_unused]] void underlying_type(compiler::type* value);
protected:
    type_number_class_t on_number_class() const override;

    type_access_model_t on_access_model() const override;

	bool on_initialize(result& r, compiler::program* program) override;

private:
	compiler::type* underlying_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ANY_TYPE_H_
