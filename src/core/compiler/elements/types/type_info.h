//
// Created by 12132 on 2022/7/17.
//

#ifndef COMPILER_ELEMENTS_TYPE_INFO_H_
#define COMPILER_ELEMENTS_TYPE_INFO_H_
#include "composite_type.h"
namespace gfx::compiler {
class type_info : public compiler::composite_type {
public:
    type_info(block* parent_scope, block* scope);

protected:
    bool on_initialize(result& r, compiler::program* program) override;
};
}

#endif // COMPILER_ELEMENTS_TYPE_INFO_H_
