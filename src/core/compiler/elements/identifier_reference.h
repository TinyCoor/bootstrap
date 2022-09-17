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
    bool on_is_constant() const override;

    bool on_as_bool(bool &value) const override;

    bool on_as_float(double &value) const override;

    bool on_as_integer(uint64_t &value) const override;

    bool on_as_string(std::string &value) const override;

    bool on_emit(result& r, emit_context_t& context) override;

    compiler::type* on_infer_type(const compiler::program* program) override;

private:
    qualified_symbol_t symbol_;
    compiler::identifier* identifier_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_IDENTIFIER_REFERENCE_H_
