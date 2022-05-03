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
	binary_operator(element* parent, operator_type_t type, expression* lhs, expression* rhs);

	~binary_operator() override;

	expression* lhs();

	expression* rhs();

private:
	expression* lhs_ = nullptr;
	expression* rhs_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_BINARY_OPERATOR_H_
