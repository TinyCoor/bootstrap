//
// Created by 12132 on 2022/11/4.
//

#ifndef INTRINICS_TYPE_OF_INTRINSIC_H_
#define INTRINICS_TYPE_OF_INTRINSIC_H_
#include "intrinsic.h"
namespace gfx::compiler {
class type_of_intrinsic : public intrinsic {
public:
    type_of_intrinsic(compiler::module* module, compiler::block* parent_scope,
        compiler::argument_list* args);

    std::string_view name() const override;

protected:
    bool on_is_constant() const override;

    bool on_fold(compiler::session& session, fold_result_t& result) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t &result) override;
};
}

#endif // INTRINICS_TYPE_OF_INTRINSIC_H_
