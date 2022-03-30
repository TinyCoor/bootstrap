//
// Created by 12132 on 2022/3/30.
//

#include "alpha_parser.h"
namespace gfx {
alpha_parser::alpha_parser() : parser() {
}

alpha_parser::~alpha_parser() {
}

void alpha_parser::parse_program() {
	auto token = current_token();
	if (token == nullptr)
		return;

	while (true) {
		auto scope = current_scope();

		token = current_token();
		if (token == nullptr || is_failed())
			break;

		consume_white_space();

		auto comment_node = parse_comment();
		if (comment_node != nullptr) {
			scope->children.push_back(comment_node);
			continue;
		}

		auto statement_node = parse_statement();
		if (statement_node == nullptr)
			break;
		scope->children.push_back(statement_node);
	}
}

ast_node_shared_ptr alpha_parser::pop_scope() {
	if (scope_stack_.empty())
		return nullptr;
	scope_stack_.pop();
	return scope_stack_.top();
}

ast_node_shared_ptr alpha_parser::parse_statement() {
	consume_white_space();

	auto statement_node = create_ast_node(ast_node_t::tokens::statement);

	auto identifier_node = parse_identifier();
	if (identifier_node != nullptr) {
		auto assignment_node = parse_assignment();
		if (assignment_node != nullptr) {
			assignment_node->lhs = identifier_node;
			statement_node->children.push_back(assignment_node);
		}
	}

	auto semicolon_literal = parse_semicolon_literal();
	if (semicolon_literal == nullptr) {
		error("A977", "statement must be terminated with a semi-colon.");
		return nullptr;
	}

	return statement_node;
}

ast_node_shared_ptr alpha_parser::current_scope() const {
	if (scope_stack_.empty())
		return nullptr;
	auto top = scope_stack_.top();
	return top;
}

ast_node_shared_ptr alpha_parser::parse(const parser_input_t& input) {
	if (input.empty())
		return nullptr;

	auto program_node = create_ast_node(ast_node_t::tokens::program);
	push_scope(program_node);
	reset(input);
	parse_program();

	return current_scope();
}

ast_node_shared_ptr alpha_parser::push_scope(const ast_node_shared_ptr& node) {
	scope_stack_.push(node);
	return scope_stack_.top();
}

}