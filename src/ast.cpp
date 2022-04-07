//
// Created by 12132 on 2022/4/1.
//

#include "ast.h"
namespace gfx{
ast_builder::ast_builder() {
}

ast_builder::~ast_builder() {
}

ast_node_shared_ptr ast_builder::pop_scope() {
	if (scope_stack_.empty())
		return nullptr;
	auto top = scope_stack_.top();
	scope_stack_.pop();
	return top;
}

ast_node_t* ast_builder::current_scope() const {
	if (scope_stack_.empty())
		return nullptr;
	return scope_stack_.top().get();
}

ast_node_shared_ptr ast_builder::program_node() {
	auto node = std::make_shared<ast_node_t>();
	node->type = ast_node_types_t::program;
	push_scope(node);
	return node;
}

ast_node_shared_ptr ast_builder::end_scope() {
	return pop_scope();
}

ast_node_shared_ptr ast_builder::begin_scope() {
	if (scope_stack_.empty()) {
		return program_node();
	} else {
		return basic_block_node();
	}
}

ast_node_shared_ptr ast_builder::statement_node() {
	auto node = std::make_shared<ast_node_t>();
	node->type = ast_node_types_t::statement;
	return node;
}

ast_node_shared_ptr ast_builder::assignment_node() {
	auto node = std::make_shared<ast_node_t>();
	node->type = ast_node_types_t::assignment;
	return node;
}

ast_node_shared_ptr ast_builder::basic_block_node() {
	auto node = std::make_shared<ast_node_t>();
	node->type = ast_node_types_t::basic_block;
	push_scope(node);
	return node;
}

void ast_builder::push_scope(const ast_node_shared_ptr& node) {
	scope_stack_.push(node);
}

ast_node_shared_ptr ast_builder::attribute_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::attribute;
	return node;
}

ast_node_shared_ptr ast_builder::null_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::null_literal;
	return node;
}

ast_node_shared_ptr ast_builder::line_comment_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::line_comment;
	return node;
}

ast_node_shared_ptr ast_builder::block_comment_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::block_comment;
	return node;
}

ast_node_shared_ptr ast_builder::string_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::string_literal;
	return node;
}

ast_node_shared_ptr ast_builder::number_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::number_literal;
	return node;
}

ast_node_shared_ptr ast_builder::boolean_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::boolean_literal;
	return node;
}

ast_node_shared_ptr ast_builder::type_identifier_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::type_identifier;
	return node;
}

ast_node_shared_ptr ast_builder::character_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::character_literal;
	return node;
}

ast_node_shared_ptr ast_builder::variable_reference_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::variable_reference;
	return node;
}

ast_node_shared_ptr ast_builder::variable_declaration_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	node->token = token;
	node->type = ast_node_types_t::variable_declaration;
	return node;
}

}