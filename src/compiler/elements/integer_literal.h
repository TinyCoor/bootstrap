//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#define COMPILER_ELEMENTS_INTEGER_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class integer_literal : public element {
public:
	explicit integer_literal(uint64_t value);

	~integer_literal() override;

	inline uint64_t value() const {
		return value_;
	}

private:
	uint64_t value_;
};
}

#endif // COMPILER_ELEMENTS_INTEGER_LITERAL_H_
