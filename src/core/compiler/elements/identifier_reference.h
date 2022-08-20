//
// Created by 12132 on 2022/8/20.
//

#ifndef COMPILER_ELEMENTS_IDENTIFIER_REFERENCE_H_
#define COMPILER_ELEMENTS_IDENTIFIER_REFERENCE_H_
#include "element.h"
namespace gfx::compiler {
class identifier_reference : public element {
public:
    identifier_reference(block* parent_scope, const qualified_symbol_t& symbol,
        compiler::identifier* identifier);

    bool resolved() const;

    compiler::identifier* identifier();

    const qualified_symbol_t& symbol() const;

    void identifier(compiler::identifier* value);
protected:
    bool on_emit(result& r, emit_context_t& context) override;

    bool on_is_constant() const override;
private:
    qualified_symbol_t symbol_;
    compiler::identifier* identifier_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_IDENTIFIER_REFERENCE_H_
