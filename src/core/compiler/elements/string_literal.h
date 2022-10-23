//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_STRING_LITERAL_H_
#define COMPILER_ELEMENTS_STRING_LITERAL_H_
#include "element.h"
namespace gfx::compiler {
class string_literal : public element {
public:
	string_literal(compiler::module* module, block* parent_scope, const std::string& value);

	std::string value() const;

    std::string escaped_value() const;

protected:
    bool on_is_constant() const override;

    bool on_emit(compiler::session &session) override;

    bool on_as_string(std::string &value) const override;

    bool on_infer_type(const compiler::session& session, type_inference_result_t& result) override;

private:
	std::string value_;
};
}

#endif // COMPILER_ELEMENTS_STRING_LITERAL_H_
