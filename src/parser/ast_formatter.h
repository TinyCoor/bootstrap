//
// Created by 12132 on 2022/4/7.
//

#ifndef AST_FORMATTER_H_
#define AST_FORMATTER_H_
#include "ast.h"

namespace gfx {
class ast_formatter {
public:
	explicit ast_formatter(const ast_node_shared_ptr& root, FILE* file);

	void format_text();

	void format_graph_viz();

private:
	void format_text_node(const ast_node_shared_ptr& node, uint32_t level);

	void format_graph_viz_node(const ast_node_shared_ptr& node);

	std::string get_vertex_name(const ast_node_shared_ptr& node) const;

private:
	FILE* file_ = nullptr;
	ast_node_shared_ptr root_;
};
}

#endif // AST_FORMATTER_H_
