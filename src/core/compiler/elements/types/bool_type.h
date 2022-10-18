//
// Created by 12132 on 2022/9/24.
//

#ifndef COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
#define COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class bool_type : public compiler::type {
public:
    explicit bool_type(compiler::module* module, block* parent_scope);
protected:
    type_number_class_t on_number_class() const override;

    type_access_model_t on_access_model() const override;

    bool on_type_check(compiler::type* other) override;

    bool on_initialize(compiler::session& session) override;
private:

};
}
#endif // COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
