//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_DIRECTIVE_H_
#define COMPILER_ELEMENTS_DIRECTIVE_H_
#include "block.h"
#include "../session.h"
namespace gfx::compiler {
class directive : public element {
public:
	using directive_callable = std::function<bool(compiler::directive*, result&, compiler::session&, compiler::program*)>;

    directive(block* parent_scope, const std::string& name, element* expression);

	element* expression();

	std::string name() const;

	bool evaluate(result& r, compiler::session &session, compiler::program* program);

	bool execute(result& r, compiler::session &session,compiler::program* program);
protected:
    void on_owned_elements(element_list_t& list) override;

private:
	bool on_execute_run(result& r, compiler::session &session, compiler::program* program);

	bool on_evaluate_run(result& r, compiler::session &session, compiler::program* program);

	bool on_execute_foreign(result& r, compiler::session &session, compiler::program* program);

	bool on_evaluate_foreign(result& r, compiler::session &session, compiler::program* program);

private:
	static inline std::unordered_map<std::string, directive_callable> s_evaluate_handlers = {
		{"run",     std::bind(&directive::on_evaluate_run,     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)},
		{"foreign", std::bind(&directive::on_evaluate_foreign, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)},
	};

	static inline std::unordered_map<std::string, directive_callable> s_execute_handlers = {
		{"run",     std::bind(&directive::on_execute_run,     std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)},
		{"foreign", std::bind(&directive::on_execute_foreign, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)},
	};

private:
	std::string name_;
	element* expression_ = nullptr;
};
}

#endif // COMPILER_ELEMENTS_DIRECTIVE_H_
