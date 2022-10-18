//
// Created by 12132 on 2022/9/14.
//

#ifndef COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
#define COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class module_type : public compiler::composite_type {
public:
    explicit module_type(compiler::module* module, block* parent_scope, block* scope);
protected:
    bool on_type_check(compiler::type* other) override;

    bool on_initialize(compiler::session& session) override;
};
}

#endif //COMPILER_ELEMENTS_TYPES_MODULE_TYPE_H_
