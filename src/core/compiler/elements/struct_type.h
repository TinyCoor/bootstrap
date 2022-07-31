//
// Created by 12132 on 2022/7/17.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
#include "composite_type.h"
#include "element.h"
namespace gfx::compiler {
class struct_type : public composite_type {
public:
    struct_type(block* parent_scope, const std::string &name);
private:
protected:
    bool on_initialize(result &r, compiler::program *program) override;

};
}
#endif //BOOTSTRAP_SRC_CORE_COMPILER_ELEMENTS_STRUCT_TYPE_H_
