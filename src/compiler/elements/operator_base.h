//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_OPERATOR_BASE_H_
#define COMPILER_ELEMENTS_OPERATOR_BASE_H_
#include "element.h"
namespace gfx::compiler {
enum class operator_type_t {
	// unary
	negate,
	binary_not,
	logical_not,

	// binary
	add,
	subtract,
	multiply,
	divide,
	modulo,
	equals,
	not_equals,
	greater_than,
	less_than,
	greater_than_or_equal,
	less_than_or_equal,
	logical_or,
	logical_and,
	binary_or,
	binary_and,
	binary_xor,
	shift_right,
	shift_left,
	rotate_right,
	rotate_left,
	exponent,
	assignment
};

class operator_base : public  element {
public:
	explicit  operator_base(operator_type_t type);
	~operator_base() override;

	operator_type_t type() const;

private:
	operator_type_t type_;

};
}

#endif // COMPILER_ELEMENTS_OPERATOR_BASE_H_
