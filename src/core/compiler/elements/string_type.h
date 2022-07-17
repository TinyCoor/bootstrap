//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_STRING_TYPE_H_
#define COMPILER_ELEMENTS_STRING_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class string_type : public composite_type {
public:
	explicit string_type(element* parent);

protected:
    bool on_initialize(result& r, compiler::program* program) override;
};
}

#endif // COMPILER_ELEMENTS_STRING_TYPE_H_
