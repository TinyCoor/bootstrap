//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_OPERATOR_BASE_H_
#define COMPILER_ELEMENTS_OPERATOR_BASE_H_
#include "element.h"
namespace gfx::compiler {

class operator_base : public element {
public:
	explicit operator_base(compiler::module* module, block* parent_scope, element_type_t element_type, operator_type_t type);

	operator_type_t operator_type() const;

private:
	operator_type_t operator_type_;
};
}

#endif // COMPILER_ELEMENTS_OPERATOR_BASE_H_
