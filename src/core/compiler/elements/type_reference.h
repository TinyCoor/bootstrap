//
// Created by 12132 on 2022/11/4.
//

#ifndef CORE_COMPILER_TYPE_REFERENCE_H_
#define CORE_COMPILER_TYPE_REFERENCE_H_
#include "element.h"

namespace gfx::compiler {
class type_reference : public element {
public:
    type_reference(compiler::module* module, block* parent_scope,
                   const qualified_symbol_t& symbol, compiler::type* type);

    bool resolved() const;

    compiler::type* type();

    void type(compiler::type* value);

    const qualified_symbol_t& symbol() const;


protected:
    bool on_is_constant() const override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
    qualified_symbol_t symbol_;
    compiler::type* type_ = nullptr;
};
}
#endif //CORE_COMPILER_TYPE_REFERENCE_H_
