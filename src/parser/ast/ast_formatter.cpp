//
// Created by 12132 on 2022/4/7.
//

#include "ast_formatter.h"
#include "fmt/format.h"
#include <set>
namespace gfx {

ast_formatter::ast_formatter(const ast_node_shared_ptr& root)
	: root_(root)
{
}

void ast_formatter::format_text() {
	format_text_node(root_, 0);
}

void ast_formatter::format_text_node(const ast_node_shared_ptr &node, uint32_t level) {
	if (node == nullptr) {
		fmt::print("nullptr");
		return;
	}
	fmt::print(
		"[type: {} | token: {}]\n",
		node->name(),
		node->token.name());
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
	format_text_node(node->lhs, level + 7);
	fmt::print("\n");
	fmt::print("{1:{0}}             --\n", level, "");
	fmt::print("{1:{0}}       rhs: ", level, "");
	format_text_node(node->rhs, level + 7);
	fmt::print("\n");
	fmt::print("{1:{0}}             --\n", level, "");

	auto index = 0;
	for (auto child : node->children) {
		fmt::print("{1:{0}}      [{2:02}] ", level, "", index++);
		format_text_node(child, level + 6);
		fmt::print("\n");
	}
}

void ast_formatter::format_graph_viz() {
	fmt::print("digraph {{\n");
	//fmt::print("rankdir=LR\n");
	fmt::print("splines=\"line\";\n");
	format_graph_viz_node(root_);
	fmt::print("}}\n");
}

void ast_formatter::format_graph_viz_node(const ast_node_shared_ptr& node) {
	auto node_vertex_name = get_vertex_name(node);

	std::string shape = "record", style, details;

	switch (node->type) {
		case ast_node_types_t::line_comment:
		case ast_node_types_t::block_comment:
			style = ", fillcolor=green, style=\"filled\"";
			details = fmt::format("|{{ token: '{}' }}", node->token.value);
			break;
		case ast_node_types_t::program:
			style = ", fillcolor=cadetblue, style=\"filled\"";
			break;
		case ast_node_types_t::binary_operator:
			style = ", fillcolor=goldenrod1, style=\"filled\"";
			break;
		case ast_node_types_t::symbol_reference:
			style = ", fillcolor=aquamarine3, style=\"filled\"";
			break;
		case ast_node_types_t::type_identifier:
			style = ", fillcolor=gainsboro, style=\"filled\"";
			break;
		case ast_node_types_t::attribute:
			style = ", fillcolor=darkseagreen, style=\"filled\"";
			break;
		case ast_node_types_t::statement:
			style = ", fillcolor=cornflowerblue, style=\"filled\"";
			break;
		case ast_node_types_t::fn_expression:
			style = ", fillcolor=cyan, style=\"filled\"";
			break;
		case ast_node_types_t::fn_call:
			style = ", fillcolor=darkorchid1, style=\"filled\"";
			break;
		case ast_node_types_t::basic_block:
			style = ", fillcolor=lightblue, style=\"filled\"";
			break;
		case ast_node_types_t::assignment:
			style = ", fillcolor=pink, style=\"filled\"";
			break;
		case ast_node_types_t::if_expression:
		case ast_node_types_t::else_expression:
		case ast_node_types_t::elseif_expression:
			shape = "Mrecord";
			style = ", fillcolor=yellow, style=\"filled\"";
			break;
		default:
			if (!node->token.value.empty()) {
				details = fmt::format(
					"|{{ token: '{}' | radix: {} | is_pointer: {} | is_array: {} }}",
					node->token.value,
					node->token.radix,
					node->is_pointer(),
					node->is_array());
			}
			break;
	}

	fmt::print(
		"{}[shape={},label=\"<f0> lhs|<f1> {}{}|<f2> rhs\"{}];\n",
		node_vertex_name,
		shape,
		node->name(),
		details,
		style);
	if (node->lhs != nullptr) {
		format_graph_viz_node(node->lhs);
		fmt::print(
			"{}:f0 -> {}:f1;\n",
			node_vertex_name,
			get_vertex_name(node->lhs));
	}

	if (node->rhs != nullptr) {
		format_graph_viz_node(node->rhs);
		fmt::print(
			"{}:f2 -> {}:f1;\n",
			node_vertex_name,
			get_vertex_name(node->rhs));
	}

	auto index = 0;
	std::set<std::string> edges {};

	for (auto child : node->children) {
		format_graph_viz_node(child);
		edges.insert(get_vertex_name(child));
		index++;
	}

	if (!edges.empty()) {
		index = 0;
		for (const auto& edge : edges)
			fmt::print(
				"{}:f1 -> {}:f1 [label=\"[{:02}]\"];\n",
				node_vertex_name,
				edge,
				index++);
		fmt::print("\n");
	}
}

std::string ast_formatter::get_vertex_name(const ast_node_shared_ptr& node) const {
	return fmt::format("{}{}", node->name(), node->id);
}

}