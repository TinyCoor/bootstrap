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

    std::string_view name() const override;

protected:
    bool on_is_constant() const override;

    bool on_fold(compiler::session& session, fold_result_t& result) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t &result) override;

};
}

#endif // COMPILER_ELEMENTS_SIZE_OF_INTRINSIC_H_
