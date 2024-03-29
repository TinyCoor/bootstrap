//
// Created by 12132 on 2022/9/17.
//

#ifndef COMPILER_ELEMENTS_TYPES_POINTER_TYPE_H_
#define COMPILER_ELEMENTS_TYPES_POINTER_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class pointer_type : public compiler::type {
public:
    static std::string name_for_pointer(compiler::type* base_type);

    pointer_type(compiler::module* module, compiler::block* parent_scope, compiler::type* base_type);

    compiler::type* base_type() const;

protected:
    bool on_type_check(compiler::type* other) override;

    type_number_class_t on_number_class() const override;

    type_access_model_t on_access_model() const override;

    bool on_initialize(compiler::session& session) override;

private:
    compiler::type* base_type_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_TYPES_POINTER_TYPE_H_
