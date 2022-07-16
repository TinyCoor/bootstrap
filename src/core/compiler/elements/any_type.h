//
// Created by 12132 on 2022/5/3.
//

#ifndef COMPILER_ELEMENTS_ANY_TYPE_H_
#define COMPILER_ELEMENTS_ANY_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class any_type : public type {
public:
	explicit any_type(element * parent);

	compiler::type* underlying_type();

	void underlying_type(compiler::type* value);
protected:
	bool on_initialize(result& r) override;

private:
	compiler::type* underlying_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_ANY_TYPE_H_
