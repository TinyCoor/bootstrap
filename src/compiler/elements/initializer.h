//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_INITIALIZER_H_
#define COMPILER_ELEMENTS_INITIALIZER_H_
#include "expression.h"

namespace gfx::compiler {
class initializer : public expression {
public:
	explicit initializer(element* parent, element* expr);

};
}

#endif //COMPILER_ELEMENTS_INITIALIZER_H_
