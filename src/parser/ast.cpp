//
// Created by 12132 on 2022/4/1.
//

#include "ast.h"
namespace gfx{

template<ast_node_types_t type>
ast_node_shared_ptr make_ast_node() {
	auto node = std::make_shared<ast_node_t>();
	node->type = type;
	return node;
}

template<ast_node_types_t type>
ast_node_shared_ptr make_ast_node(const token_t &token) {
	auto node = std::make_shared<ast_node_t>();
	node->type = type;
	node->token = token;
	return node;
}

ast_builder::ast_builder() {
}

ast_builder::~ast_builder() {
}

ast_node_shared_ptr ast_builder::pop_scope() {
	if (scope_stack_.empty()) {
		return nullptr;
	}

	auto top = scope_stack_.top();
	scope_stack_.pop();
	return top;
}

ast_node_t* ast_builder::current_scope() const {
	if (scope_stack_.empty()) {
		return nullptr;
	}

	return scope_stack_.top().get();
}

ast_node_shared_ptr ast_builder::program_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::program;
	push_scope(node);
	return node;
}

ast_node_shared_ptr ast_builder::proc_call_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::proc_call;
	node->rhs = argument_list_node();
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
	node->id= ++id_;
	node->type = ast_node_types_t::statement;
	return node;
}

ast_node_shared_ptr ast_builder::assignment_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->type = ast_node_types_t::assignment;
	return node;
}

ast_node_shared_ptr ast_builder::basic_block_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->type = ast_node_types_t::basic_block;
	push_scope(node);
	return node;
}

void ast_builder::push_scope(const ast_node_shared_ptr& node) {
	scope_stack_.push(node);
}

ast_node_shared_ptr ast_builder::attribute_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::attribute);
	return node;
}

ast_node_shared_ptr ast_builder::null_literal_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::null_literal);
	return node;
}

ast_node_shared_ptr ast_builder::line_comment_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::line_comment);
	return node;
}

ast_node_shared_ptr ast_builder::block_comment_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::block_comment);
	return node;
}

ast_node_shared_ptr ast_builder::string_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::string_literal);
	return node;
}

ast_node_shared_ptr ast_builder::number_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::number_literal);
	return node;
}

ast_node_shared_ptr ast_builder::boolean_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::boolean_literal);
	return node;
}

ast_node_shared_ptr ast_builder::type_identifier_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::type_identifier);
	return node;
}

ast_node_shared_ptr ast_builder::character_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::character_literal);
	return node;
}

ast_node_shared_ptr ast_builder::symbol_reference_node(const token_t &token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::symbol_reference);
	return node;
}

ast_node_shared_ptr ast_builder::expression_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::expression;
	return node;
}

ast_node_shared_ptr ast_builder::argument_list_node() {
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::argument_list;
	return node;
}

ast_node_shared_ptr ast_builder::binary_operator_node(const ast_node_shared_ptr &lhs, const token_t &token,
													  const ast_node_shared_ptr &rhs) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::binary_operator);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

ast_node_shared_ptr ast_builder::unary_operator_node(const token_t &token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::unary_operator);
	return node;
}

ast_node_shared_ptr ast_builder::if_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type  = ast_node_types_t::if_expression;
	return node;
}

ast_node_shared_ptr ast_builder::else_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type  = ast_node_types_t::else_expression;
	return node;
}

ast_node_shared_ptr ast_builder::proc_expression_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type = ast_node_types_t::proc_expression;
	node->lhs = argument_list_node();
	node->rhs = argument_list_node();
	return node;
}
ast_node_shared_ptr ast_builder::else_if_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type  = ast_node_types_t::elseif_expression;
	return node;
}

ast_node_shared_ptr ast_builder::break_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::break_statement);
	return node;
}

ast_node_shared_ptr ast_builder::continue_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::continue_statement);
	return node;
}

ast_node_shared_ptr ast_builder::return_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type  = ast_node_types_t::return_statement;
	node->rhs = argument_list_node();
	return node;
}

ast_node_shared_ptr ast_builder::for_in_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type = ast_node_types_t::for_in_statement;
	return node;
}

ast_node_shared_ptr ast_builder::enum_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::enum_expression);
	return node;
}

ast_node_shared_ptr ast_builder::struct_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::struct_expression);
	return node;
}
ast_node_shared_ptr ast_builder::directive_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::directive);
	return node;
}
ast_node_shared_ptr ast_builder::qualified_symbol_reference_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->lhs = argument_list_node();
	node->type = ast_node_types_t::qualified_symbol_reference;
	return node;
}

ast_node_shared_ptr ast_builder::namespace_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::namespace_expression);
	return node;
}

ast_node_shared_ptr ast_builder::union_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::union_expression);
	return node;
}

ast_node_shared_ptr ast_builder::subscript_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::subscript_expression;
	return node;
}

void ast_builder::configure_node(const ast_node_shared_ptr &node, const token_t &token, ast_node_types_t type)
{
	node->id = ++id_;
	node->type = type;
	node->token = token;
}

ast_node_shared_ptr ast_builder::constant_node(const token_t &token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::constant_expression);
	node->lhs = argument_list_node();
	return node;
}

ast_node_shared_ptr ast_builder::with_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::with_expression);
	return node;
}

ast_node_shared_ptr ast_builder::defer_node(const token_t& token) {
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::defer_expression);
	return node;
}

ast_node_shared_ptr ast_builder::label_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::label_list;
	return node;
}

ast_node_shared_ptr ast_builder::label_node(token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::label);
	return node;
}

ast_node_shared_ptr ast_builder::alias_node(token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::alias_expression);
	return node;
}

ast_node_shared_ptr ast_builder::cast_node(token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::cast_expression);
	return node;
}

}