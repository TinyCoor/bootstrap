//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_UNARY_OPERATOR_H_
#define COMPILER_ELEMENTS_UNARY_OPERATOR_H_
#include "operator_base.h"
#include "element_types.h"

namespace gfx::compiler {
class unary_operator : public  operator_base {
public:
	explicit unary_operator(element* parent, operator_type_t type, element* rhs);

	element* rhs();

private:
	element* rhs_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_UNARY_OPERATOR_H_
