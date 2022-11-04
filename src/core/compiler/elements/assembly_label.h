//
// Created by 12132 on 2022/11/4.
//

#ifndef COMPILER_ELEMENTS_ASSEMBLY_LABEL_H_
#define COMPILER_ELEMENTS_ASSEMBLY_LABEL_H_
#include "element.h"
namespace gfx::compiler {
class assembly_label : public element {
public:
    assembly_label(compiler::module* module, block* parent_scope, const std::string& name);

    std::string name() const;

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session& session) override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
    std::string name_;
};
}
#endif //COMPILER_ELEMENTS_ASSEMBLY_LABEL_H_
