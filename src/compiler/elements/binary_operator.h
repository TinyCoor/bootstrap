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

	~binary_operator() override;

	element* lhs();

	element* rhs();

private:
	element* lhs_ = nullptr;
	element* rhs_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_BINARY_OPERATOR_H_
