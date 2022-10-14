//
// Created by 12132 on 2022/10/14.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
#include "element.h"
namespace gfx::compiler {
class transmute : public element {
public:
    transmute(block* parent_scope, compiler::type* type, element* expr);

    compiler::type* type();

    element* expression();

protected:
    bool on_emit(result& r, emit_context_t& context) override;

    void on_owned_elements(element_list_t& list) override;

    compiler::type* on_infer_type(const compiler::program* program) override;

private:
    element* expression_ = nullptr;
    compiler::type* type_ = nullptr;
};
}

#endif //BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
