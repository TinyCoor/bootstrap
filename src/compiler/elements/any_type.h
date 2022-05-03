//
// Created by 12132 on 2022/5/3.
//

#ifndef BOOTSTRAP_SRC_COMPILER_ELEMENTS_ANY_TYPE_H_
#define BOOTSTRAP_SRC_COMPILER_ELEMENTS_ANY_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class any_type : public type {
public:
	explicit any_type(element * parent);

private:

};
}

#endif //BOOTSTRAP_SRC_COMPILER_ELEMENTS_ANY_TYPE_H_
