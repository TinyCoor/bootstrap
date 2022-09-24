//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_IDENTIFIER_H_
#define COMPILER_ELEMENTS_IDENTIFIER_H_
#include "../compiler_types.h"
#include "element.h"


namespace gfx::compiler {

class identifier : public element {
public:
	identifier(block* parent_scope, symbol_element* symbol, compiler::initializer* initializer);

	compiler::type* type();

    compiler::symbol_element* symbol() const;

	bool inferred_type() const;

    identifier_usage_t usage() const;

    void usage(identifier_usage_t value);

	void type(compiler::type* t);

	void inferred_type(bool value);

	compiler::initializer* initializer() const;

    void initializer(compiler::initializer* value);
protected:
    bool on_is_constant() const override;

    bool on_as_bool(bool &value) const override;

    bool on_as_float(double &value) const override;

    bool on_as_integer(uint64_t &value) const override;

    bool on_as_string(std::string &value) const override;

    void on_owned_elements(element_list_t& list) override;

    bool on_emit(result& r, emit_context_t& context) override;

    compiler::type* on_infer_type(const compiler::program* program) override;

private:
	bool inferred_type_ = false;
	compiler::type* type_ = nullptr;
    compiler::symbol_element* symbol_;
	compiler::initializer* initializer_;
    identifier_usage_t usage_ = identifier_usage_t::heap;
};
}
#endif // COMPILER_ELEMENTS_IDENTIFIER_H_
