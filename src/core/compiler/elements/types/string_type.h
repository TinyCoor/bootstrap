//
// Created by 12132 on 2022/5/2.
//

#ifndef COMPILER_ELEMENTS_STRING_TYPE_H_
#define COMPILER_ELEMENTS_STRING_TYPE_H_
#include "composite_type.h"
namespace gfx::compiler {
class string_type : public composite_type {
public:
	explicit string_type(compiler::module* module, block* parent_scope, block* scope);

protected:
    type_access_model_t on_access_model() const override;

    bool on_initialize(compiler::session& session) override;
};
}

#endif // COMPILER_ELEMENTS_STRING_TYPE_H_
