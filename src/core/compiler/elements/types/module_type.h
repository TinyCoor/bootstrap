//
// Created by 12132 on 2022/9/14.
//

#ifndef COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
#define COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class module_type : public compiler::composite_type {
public:
    explicit module_type(block* parent_scope, block* scope);
protected:
    bool on_initialize(result &r, compiler::program *program) override;
};
}

#endif //COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
