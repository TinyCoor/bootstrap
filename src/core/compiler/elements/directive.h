//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_DIRECTIVE_H_
#define COMPILER_ELEMENTS_DIRECTIVE_H_
#include "block.h"
#include "vm/instruction_block.h"
#include "../session.h"
namespace gfx::compiler {
class directive : public element {
public:
	using directive_callable = std::function<bool(compiler::directive*, compiler::session&)>;

    directive(compiler::module *module, block* parent_scope, const std::string& name, element* expression);

	element* expression();

	std::string name() const;

	bool evaluate(compiler::session &session);

	bool execute(compiler::session &session);
protected:
    bool on_emit(compiler::session& session) override;

    void on_owned_elements(element_list_t& list) override;

private:
    // --------------------
    // assembly directive
    // --------------------
    bool on_execute_assembly(compiler::session& session);

    bool on_evaluate_assembly(compiler::session& session);

	bool on_execute_run(compiler::session &session);

	bool on_evaluate_run(compiler::session &session);

	bool on_execute_foreign(compiler::session &session);

	bool on_evaluate_foreign(compiler::session &session);

private:
	std::string name_;
	element* expression_ = nullptr;
    instruction_block* instruction_block_ = nullptr;
    static std::unordered_map<std::string, directive_callable> s_evaluate_handlers;
    static std::unordered_map<std::string, directive_callable> s_execute_handlers;
};
}

#endif // COMPILER_ELEMENTS_DIRECTIVE_H_
