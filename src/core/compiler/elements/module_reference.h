//
// Created by 12132 on 2022/9/14.
//

#ifndef COMPILER_ELEMENTS_MODULE_REFERENCE_H_
#define COMPILER_ELEMENTS_MODULE_REFERENCE_H_
#include "element.h"
namespace gfx::compiler {
class module_reference : public element {
public:
    module_reference(compiler::module* module, compiler::block* parent_scope, compiler::element* expr);

    compiler::module* reference();

    compiler::element* expression();

    void reference(compiler::module* value);

protected:
    bool on_is_constant() const override;

    void on_owned_elements(gfx::compiler::element_list_t &list) override;

    compiler::type* on_infer_type(const compiler::program* program) override;
private:
    compiler::module* module_ = nullptr;
    compiler::element* expression_ = nullptr;
};
}
#endif //COMPILER_ELEMENTS_MODULE_REFERENCE_H_
