//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_EXPRESSION_H_
#define COMPILER_ELEMENTS_EXPRESSION_H_
#include "common/result.h"
#include "element.h"
#include "element_types.h"

namespace gfx::compiler {
class expression : public element {
public:
	explicit expression(element* root);

	~expression() override;

private:
	element* root_;
};
}

#endif // COMPILER_ELEMENTS_EXPRESSION_H_