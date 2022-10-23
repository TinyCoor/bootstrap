//
// Created by 12132 on 2022/10/22.
//

#ifndef COMPILER_ELEMENTS_INTRINSIC_H_
#define COMPILER_ELEMENTS_INTRINSIC_H_
#include "element.h"
namespace gfx::compiler {
class intrinsic : public element {
public:
    intrinsic(compiler::module* module, compiler::block* parent_scope,
        compiler::identifier_reference* reference, compiler::argument_list* args);

    compiler::argument_list* arguments();

    compiler::identifier_reference* reference();

    void reference(compiler::identifier_reference* value);

protected:
    void on_owned_elements(element_list_t& list) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
    compiler::argument_list* arguments_ = nullptr;
    compiler::identifier_reference* reference_ = nullptr;
};
}
#endif // COMPILER_ELEMENTS_INTRINSIC_H_
