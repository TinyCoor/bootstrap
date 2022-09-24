//
// Created by 12132 on 2022/9/24.
//

#ifndef COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
#define COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
#include "type.h"
namespace gfx::compiler {
class bool_type : public compiler::type {
public:
    explicit bool_type(block* parent_scope);
protected:
    bool on_initialize(result& r, compiler::program* program) override;
private:

};
}
#endif // COMPILER_ELEMENTS_TYPES_BOOL_TYPE_H_
