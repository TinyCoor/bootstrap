//
// Created by 12132 on 2022/10/14.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
#include "element.h"
namespace gfx::compiler {
class transmute : public element {
public:
    transmute(compiler::module* module, block* parent_scope, compiler::type* type, element* expr);

    compiler::type* type();

    element* expression();

    void type_location(const source_location& loc);

protected:
    bool on_emit(compiler::session &session) override;

    void on_owned_elements(element_list_t& list) override;

    compiler::type* on_infer_type(const compiler::session& session) override;

private:
    element* expression_ = nullptr;
    compiler::type* type_ = nullptr;
    source_location type_location_;
};
}

#endif //BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_TRANSMUTE_H_
