//
// Created by 12132 on 2022/3/30.
//

#ifndef BOOTSTRAP_SRC_EVALUATOR_H_
#define BOOTSTRAP_SRC_EVALUATOR_H_
#include "parser.h"
#include "symbol_table.h"

namespace gfx {
class alpha_compiler;

class evaluator {
public:
	explicit evaluator(alpha_compiler *compiler);

	bool evaluate_program(
		result &result,
		const ast_node_shared_ptr &program_node);

	variant_t evaluate(
		result &result,
		const ast_node_shared_ptr &node);

	symbol_table *symbol_table();

	void symbol_table(class symbol_table *value);

protected:
	void error(
		result &result,
		const std::string &code,
		const std::string &message);

private:
	bool transform_program(
		result &result,
		const ast_node_shared_ptr &node);

	bool transform_identifier_node(
		result &result,
		const ast_node_shared_ptr &node);

private:
	alpha_compiler *compiler_ = nullptr;
	class symbol_table *symbol_table_ = nullptr;
};
}


#endif //BOOTSTRAP_SRC_EVALUATOR_H_
