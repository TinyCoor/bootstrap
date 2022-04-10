//
// Created by 12132 on 2022/4/7.
//

#include "ast_formatter.h"
#include "fmt/format.h"
namespace gfx {

ast_formatter::ast_formatter(const ast_node_shared_ptr& root)
	: root_(root)
{
}

void ast_formatter::format()
{
	format_node(root_, 0);
}

void ast_formatter::format_node(const ast_node_shared_ptr& node, uint32_t level)
{
	if (node == nullptr) {
		fmt::print("nullptr");
		return;
	}
	fmt::print("[type: {} | token: {}]\n", node->name(), node->token.name());
	fmt::print("{1:{0}}     value: '{2}'\n", level, "", node->token.value);
	if (node->token.is_numeric()) {
		fmt::print("{1:{0}}     radix: {2}\n", level, "", node->token.radix);
		switch (node->token.number_type) {
			case number_types_t::none:
				fmt::print("{1:{0}}      type: none\n", level, "");
				break;
			case number_types_t::integer:
				fmt::print("{1:{0}}      type: integer\n", level, "");
				break;
			case number_types_t::floating_point:
				fmt::print("{1:{0}}      type: floating_point\n", level, "");
				break;
		}
	}
	fmt::print("{1:{0}}is_pointer: {2}\n", level, "", node->is_pointer());
	fmt::print("{1:{0}}  is_array: {2}\n", level, "", node->is_array());
	fmt::print("{1:{0}}             --\n", level, "");
	fmt::print("{1:{0}}       lhs: ", level, "");
	format_node(node->lhs, level + 7);
	fmt::print("\n");
	fmt::print("{1:{0}}             --\n", level, "");
	fmt::print("{1:{0}}       rhs: ", level, "");
	format_node(node->rhs, level + 7);
	fmt::print("\n");
	fmt::print("{1:{0}}             --\n", level, "");

	auto index = 0;
	for (auto child : node->children) {
		fmt::print("{1:{0}}      [{2:02}] ", level, "", index++);
		format_node(child, level + 6);
		fmt::print("\n");
	}
}

}