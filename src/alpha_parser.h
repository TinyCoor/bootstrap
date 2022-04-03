//
// Created by 12132 on 2022/3/30.
//

#ifndef ALPHA_PARSER_H_
#define ALPHA_PARSER_H_
#include "parser.h"
#include "ast.h"

namespace gfx {
class alpha_parser : public parser {
public:
	alpha_parser();

	virtual ~alpha_parser();

private:
	std::stack<ast_node_shared_ptr> scope_stack_{};
};
}

#endif // ALPHA_PARSER_H_
