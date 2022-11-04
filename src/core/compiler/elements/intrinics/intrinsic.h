//
// Created by 12132 on 2022/10/22.
//

#ifndef COMPILER_ELEMENTS_INTRINSIC_H_
#define COMPILER_ELEMENTS_INTRINSIC_H_
#include "core/compiler/elements/element.h"

namespace gfx::compiler {
class intrinsic : public element {
public:
    static intrinsic* intrinsic_for_call(compiler::session& session,
        compiler::block* parent_scope, compiler::argument_list* args,
        const qualified_symbol_t& symbol);

    intrinsic(compiler::module* module, compiler::block* parent_scope, compiler::argument_list* args);

    virtual std::string_view name() const;

    compiler::argument_list* arguments();
protected:
    void on_owned_elements(element_list_t& list) override;

private:
    compiler::argument_list* arguments_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_INTRINSIC_H_
