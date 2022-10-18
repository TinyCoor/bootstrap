//
// Created by 12132 on 2022/8/4.
//

#ifndef COMPILER_ELEMENTS_TUPLE_TYPE_H_
#define COMPILER_ELEMENTS_TUPLE_TYPE_H_
#include "core/compiler/elements/types/composite_type.h"
namespace gfx::compiler {
class tuple_type : public composite_type {
public:
    explicit tuple_type(compiler::module* module, block* parent_scope, block* scope);

protected:
    type_access_model_t on_access_model() const override;
    bool on_initialize(compiler::session& session) override;
};
}

#endif //COMPILER_ELEMENTS_TUPLE_TYPE_H_
