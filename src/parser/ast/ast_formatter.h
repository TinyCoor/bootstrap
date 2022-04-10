//
// Created by 12132 on 2022/4/7.
//

#ifndef AST_FORMATTER_H_
#define AST_FORMATTER_H_
#include "ast.h"

namespace gfx {
class ast_formatter {
public:
	explicit ast_formatter(const ast_node_shared_ptr& root);

	void format();

private:
	void format_node(const ast_node_shared_ptr& node, uint32_t level);

private:
	ast_node_shared_ptr root_;
};
}

#endif // AST_FORMATTER_H_
