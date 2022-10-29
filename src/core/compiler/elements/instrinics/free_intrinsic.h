//
// Created by 12132 on 2022/10/29.
//

#ifndef COMPILER_ELEMENTS_INSTRINICS_FREE_INSTRINSIC_H_
#define COMPILER_ELEMENTS_INSTRINICS_FREE_INSTRINSIC_H_
#include "intrinsic.h"
namespace gfx::compiler {
class free_intrinsic : public compiler::intrinsic {
public:
    free_intrinsic(compiler::module* module, compiler::block* parent_scope,
        compiler::argument_list* args);

protected:
    bool on_emit(compiler::session& session) override;

};
}

#endif //COMPILER_ELEMENTS_INSTRINICS_FREE_INSTRINSIC_H_
