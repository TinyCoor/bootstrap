//
// Created by 12132 on 2022/10/29.
//

#ifndef COMPILER_ELEMENTS_SIZE_OF_INTRINSIC_H_
#define COMPILER_ELEMENTS_SIZE_OF_INTRINSIC_H_
#include "intrinsic.h"
namespace gfx::compiler {
class size_of_intrinsic : public intrinsic {
public:
    size_of_intrinsic(compiler::module* module, compiler::block* parent_scope,
        compiler::argument_list* args);

protected:
    bool on_is_constant() const override;

    compiler::element* on_fold(compiler::session& session) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t &result) override;

};
}

#endif // COMPILER_ELEMENTS_SIZE_OF_INTRINSIC_H_
