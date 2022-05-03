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

	~float_literal() override;

	inline double value() const {
		return value_;
	}

private:
	double value_;
};

}

#endif // COMPILER_ELEMENTS_FLOAT_LITERAL_H_
