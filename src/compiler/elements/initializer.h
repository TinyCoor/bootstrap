//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INITIALIZER_H_
#define COMPILER_ELEMENTS_INITIALIZER_H_
#include "expression.h"

namespace gfx::compiler {
class initializer : public expression {
public:
	explicit initializer(element* parent, binary_operator* assignment);

	~initializer() override;

	binary_operator* assignment();

private:
	binary_operator* assignment_ = nullptr;
};
}

#endif //COMPILER_ELEMENTS_INITIALIZER_H_
