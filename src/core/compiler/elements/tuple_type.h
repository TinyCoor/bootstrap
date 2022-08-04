//
// Created by 12132 on 2022/8/4.
//

#ifndef COMPILER_ELEMENTS_TUPLE_TYPE_H_
#define COMPILER_ELEMENTS_TUPLE_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class tuple_type : public composite_type {
public:
    explicit tuple_type(block* parent_scope);
protected:
    bool on_initialize(result &r, compiler::program *program) override;
};
}

#endif //COMPILER_ELEMENTS_TUPLE_TYPE_H_
