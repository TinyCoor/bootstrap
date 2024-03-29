//
// Created by 12132 on 2022/7/17.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
#include "composite_type.h"
#include "core/compiler/elements/element.h"
namespace gfx::compiler {
class struct_type : public composite_type {
public:
    struct_type(compiler::module* module, block* parent_scope, symbol_element* symbol, block* scope);
private:
protected:
    bool on_initialize(compiler::session& session) override;

};
}
#endif //BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
