//
// Created by 12132 on 2022/3/30.
//

#ifndef BOOTSTRAP_SRC_ALPHA_PARSER_H_
#define BOOTSTRAP_SRC_ALPHA_PARSER_H_
#include "parser.h"
namespace gfx {
class alpha_parser : public parser {
public:
	alpha_parser();

	virtual ~alpha_parser();

	ast_node_shared_ptr parse(const parser_input_t &input) override;

protected:
	void parse_program();

private:
	ast_node_shared_ptr parse_statement();

private:
	ast_node_shared_ptr pop_scope();

	ast_node_shared_ptr current_scope() const;

	ast_node_shared_ptr push_scope(const ast_node_shared_ptr &node);

private:
	std::stack<ast_node_shared_ptr> scope_stack_{};
};
}

#endif //BOOTSTRAP_SRC_ALPHA_PARSER_H_
