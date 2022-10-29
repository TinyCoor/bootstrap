//
// Created by 12132 on 2022/10/29.
//

#ifndef COMPILER_ELEMENTS_INSTRINICS_ALLOC_INTRINSIC_H_
#define COMPILER_ELEMENTS_INSTRINICS_ALLOC_INTRINSIC_H_
#include "intrinsic.h"
namespace gfx::compiler {
class alloc_intrinsic : public  compiler::intrinsic {
public:
    alloc_intrinsic(compiler::module* module, compiler::block* parent_scope,
        compiler::argument_list* args);

protected:
    bool on_emit(compiler::session& session) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t &result) override;
};
}

#endif //COMPILER_ELEMENTS_INSTRINICS_ALLOC_INTRINSIC_H_
