//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_STRING_LITERAL_H_
#define COMPILER_ELEMENTS_STRING_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class string_literal : public element {
public:
	string_literal(element* parent, const std::string& value);

	std::string value() const;

protected:
    bool on_as_string(std::string &value) const override;

    compiler::type* on_infer_type(const compiler::program* program) override;

    bool on_is_constant() const override;

    bool on_emit(result &r, assembler &assembler, emit_context_t &context) override;

private:
	std::string value_;
};
}

#endif // COMPILER_ELEMENTS_STRING_LITERAL_H_
