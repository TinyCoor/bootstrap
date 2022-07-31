//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_ANY_TYPE_H_
#define COMPILER_ELEMENTS_ANY_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class any_type : public composite_type {
public:
	explicit any_type(block * parent_scope);

	compiler::type* underlying_type();

	void underlying_type(compiler::type* value);
protected:
	bool on_initialize(result& r, compiler::program* program) override;

private:
	compiler::type* underlying_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ANY_TYPE_H_
