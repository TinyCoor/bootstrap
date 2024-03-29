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
ast_node_shared_ptr make_ast_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	node->type = type;
	node->token = token;
	return node;
}

ast_builder::ast_builder() {
}

ast_builder::~ast_builder() {
}

ast_node_shared_ptr ast_builder::pop_scope()
{
	if (scope_stack_.empty()) {
		return nullptr;
	}

	auto top = scope_stack_.top();
	scope_stack_.pop();
	return top;
}

ast_node_shared_ptr ast_builder::current_scope() const
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	return scope_stack_.top();
}

ast_node_shared_ptr ast_builder::module_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::module;
	push_scope(node);
	return node;
}

ast_node_shared_ptr ast_builder::proc_call_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::proc_call;
	node->rhs = argument_list_node();
	return node;
}

ast_node_shared_ptr ast_builder::end_scope()
{
	return pop_scope();
}

ast_node_shared_ptr ast_builder::begin_scope()
{
	if (scope_stack_.empty()) {
		return module_node();
	} else {
		return basic_block_node();
	}
}

ast_node_shared_ptr ast_builder::statement_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->type = ast_node_types_t::statement;
	return node;
}

ast_node_shared_ptr ast_builder::assignment_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->type = ast_node_types_t::assignment;
	node->lhs = assignment_target_list_node();
    node->rhs = assignment_source_list_node();
	return node;
}

ast_node_shared_ptr ast_builder::basic_block_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id= ++id_;
	node->type = ast_node_types_t::basic_block;
	push_scope(node);
	return node;
}

void ast_builder::push_scope(const ast_node_shared_ptr& node)
{
	scope_stack_.push(node);
}

ast_node_shared_ptr ast_builder::attribute_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::attribute);
	return node;
}

ast_node_shared_ptr ast_builder::null_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::null_literal);
	return node;
}

ast_node_shared_ptr ast_builder::line_comment_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::line_comment);
	return node;
}

ast_node_shared_ptr ast_builder::block_comment_node(const token_t& token)
{
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

ast_node_shared_ptr ast_builder::character_literal_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::character_literal);
	return node;
}

ast_node_shared_ptr ast_builder::symbol_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::symbol;
	return node;
}

ast_node_shared_ptr ast_builder::expression_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::expression;
	return node;
}

ast_node_shared_ptr ast_builder::argument_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::argument_list;
	return node;
}

ast_node_shared_ptr ast_builder::binary_operator_node(const ast_node_shared_ptr &lhs, const token_t &token,
													  const ast_node_shared_ptr &rhs)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::binary_operator);
	node->lhs = lhs;
	node->rhs = rhs;
    node->location.start(lhs->location.start());
    node->location.end(rhs->location.end());
	return node;
}

ast_node_shared_ptr ast_builder::unary_operator_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::unary_operator);
	return node;
}

ast_node_shared_ptr ast_builder::if_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::if_expression);
	return node;
}

ast_node_shared_ptr ast_builder::else_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::else_expression);
	return node;
}

ast_node_shared_ptr ast_builder::proc_expression_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type = ast_node_types_t::proc_expression;
	node->lhs = type_list_node();
	node->rhs = parameter_list_node();
    node->location = token.location;
	return node;
}
ast_node_shared_ptr ast_builder::else_if_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::elseif_expression);
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

ast_node_shared_ptr ast_builder::return_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	node->id =++id_;
	node->type  = ast_node_types_t::return_statement;
	node->rhs = return_argument_list_node();
    node->location = token.location;
	return node;
}

ast_node_shared_ptr ast_builder::for_in_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::for_in_statement);
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

ast_node_shared_ptr ast_builder::symbol_part_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::symbol_part);
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
    node->location = token.location;
}

ast_node_shared_ptr ast_builder::with_node(const token_t& token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::with_expression);
	return node;
}

ast_node_shared_ptr ast_builder::defer_node(const token_t& token)
{
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

ast_node_shared_ptr ast_builder::label_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::label);
	return node;
}

ast_node_shared_ptr ast_builder::cast_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::cast_expression);
	return node;
}

ast_node_shared_ptr ast_builder::pair_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::pair;
	return node;
}

ast_node_shared_ptr ast_builder::type_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::type_list;
	return node;
}

ast_node_shared_ptr ast_builder::parameter_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::parameter_list;
	return node;
}

ast_node_shared_ptr ast_builder::assignment_target_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::assignment_target_list;
	return node;
}

ast_node_shared_ptr ast_builder::return_argument_list_node()
{
	auto node = std::make_shared<ast_node_t>();
	node->id = ++id_;
	node->type = ast_node_types_t::return_argument_list;
	return node;
}

ast_node_shared_ptr ast_builder::import_node(const token_t &token)
{
	auto node = std::make_shared<ast_node_t>();
	configure_node(node, token, ast_node_types_t::import_expression);
	return node;
}
ast_node_shared_ptr ast_builder::type_identifier_node()
{
    auto node = std::make_shared<ast_node_t>();
    node->id = ++id_;
    node->type = ast_node_types_t::type_identifier;
    return node;
}

ast_node_shared_ptr ast_builder::from_node(const token_t &token)
{
    auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::from_expression);
    return node;
}

ast_node_shared_ptr ast_builder::module_expression_node(const token_t &token)
{
    auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::module_expression);
    return node;
}

ast_node_shared_ptr ast_builder::array_subscript_list_node()
{
    auto node = std::make_shared<ast_node_t>();
    node->id = ++id_;
    node->type = ast_node_types_t::array_subscript_list;
    return node;
}

ast_node_shared_ptr ast_builder::assignment_source_list_node()
{
    auto node = std::make_shared<ast_node_t>();
    node->id = ++id_;
    node->type = ast_node_types_t::assignment_source_list;
    return node;
}

ast_node_shared_ptr ast_builder::transmute_node(const token_t &token)
{
    auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::transmute_expression);
    return node;
}

ast_node_shared_ptr ast_builder::constant_assignment_node()
{
    auto node = std::make_shared<ast_node_t>();
    node->id = ++id_;
    node->type = ast_node_types_t::constant_assignment;
    node->lhs = assignment_target_list_node();
    node->rhs = assignment_source_list_node();
    return node;
}
ast_node_shared_ptr ast_builder::raw_block_node(const token_t &token)
{
    auto node = std::make_shared<ast_node_t>();
    configure_node(node, token, ast_node_types_t::raw_block);
    return node;
}

}