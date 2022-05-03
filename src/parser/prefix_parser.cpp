//
// Created by 12132 on 2022/5/2.
//

#include "prefix_parser.h"
#include "parser.h"
namespace gfx {

//////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr constant_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto constant_node = parser->ast_builder()->constant_node(token);
	constant_node->rhs = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::assignment));
	return constant_node;
}

ast_node_shared_ptr with_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto with_node = parser->ast_builder()->with_node(token);
	with_node->rhs = parser->parse_expression(r, 0);
	return with_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr defer_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto defer_node = parser->ast_builder()->defer_node(token);
	defer_node->rhs = parser->parse_expression(r, 0);
	return defer_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr union_prefix_parser::parse(result& r, parser* parser, token_t& token) {
	auto union_node = parser->ast_builder()->union_node(token);
	union_node->rhs = parser->parse_expression(r, 0);
	return union_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr namespace_prefix_parser::parse(result& r, parser* parser, token_t& token) {
	auto namespace_node = parser->ast_builder()->namespace_node(token);
	namespace_node->rhs = parser->parse_expression(r, 0);
	return namespace_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr struct_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto struct_node = parser->ast_builder()->struct_node(token);
	struct_node->rhs = parser->parse_expression(r, 0);
	return struct_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr enum_prefix_parser::parse(result& r, parser* parser, token_t& token) {
	auto enum_node = parser->ast_builder()->enum_node(token);
	enum_node->rhs = parser->parse_expression(r, 0);
	return enum_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr for_in_prefix_parser::parse(result& r, parser* parser, token_t& token) {
	auto for_node = parser->ast_builder()->for_in_node();
	for_node->lhs = parser->parse_expression(r, 0);

	token_t in_token;
	in_token.type = token_types_t::in_literal;
	if (!parser->expect(r, in_token)) {
		return nullptr;
	}

	for_node->rhs = parser->parse_expression(r, 0);
	for_node->children.push_back(parser->parse_expression(r, 0));

	return for_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr return_prefix_parser::parse(result& r, parser* parser, token_t& token) {
	auto return_node = parser->ast_builder()->return_node();

	while (true) {
		return_node->rhs->children.push_back(parser->parse_expression(r, 0));
		if (!parser->peek(token_types_t::comma))
			break;
		parser->consume();
	}

	return return_node;
}


///////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr if_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto if_node = parser->ast_builder()->if_node();
	if_node->lhs = parser->parse_expression(r, 0);
	if_node->children.push_back(parser->parse_expression(r, 0));

	auto current_branch = if_node;
	while (true) {
		if (!parser->peek(token_types_t::else_if_literal))
			break;
		parser->consume();
		current_branch->rhs = parser->ast_builder()->else_if_node();
		current_branch->rhs->lhs = parser->parse_expression(r, 0);
		current_branch->rhs->children.push_back(parser->parse_expression(r, 0));
		current_branch = current_branch->rhs;
	}

	if (parser->peek(token_types_t::else_literal)) {
		parser->consume();
		current_branch->rhs = parser->ast_builder()->else_node();
		current_branch->rhs->children.push_back(parser->parse_expression(r, 0));
	}

	return if_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr type_identifier_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return create_type_identifier_node(r, parser, token);
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr basic_block_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->parse_scope(r);
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr proc_expression_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto proc_node = parser->ast_builder()->proc_expression_node();

	token_t left_paren_token;
	left_paren_token.type = token_types_t::left_paren;
	if (!parser->expect(r, left_paren_token)){
		return nullptr;
	}

	if (!parser->peek(token_types_t::right_paren)) {
		while (true) {
			auto param_expr = parser->parse_expression(r, 0);
			if (param_expr->type ==ast_node_types_t::block_comment) {
				proc_node->children.push_back(param_expr);
				continue;
			} else {
				proc_node->rhs->children.push_back(param_expr);
			}

			if (!parser->peek(token_types_t::comma)) {
				break;
			}
			parser->consume();
		}
	}

	token_t right_paren_token;
	right_paren_token.type = token_types_t::right_paren;
	if (!parser->expect(r, right_paren_token)) {
		return nullptr;
	}

	if (parser->peek(token_types_t::colon)) {
		parser->consume();
		while (true) {
			proc_node->lhs = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::type));
			if (!parser->peek(token_types_t::comma)) {
				break;
			}
			parser->consume();
		}
	}

	if (!parser->peek(token_types_t::semi_colon)) {
		proc_node->children.push_back(parser->parse_expression(r, 0));
	}

	return proc_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr group_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto expression_node = parser->ast_builder()->expression_node();
	expression_node->lhs = parser->parse_expression(r, 0);
	token_t right_paren_token;
	right_paren_token.type = token_types_t::right_paren;
	if (!parser->expect(r, right_paren_token))
		return nullptr;
	return expression_node;
}

///////////////////////////////////////////////////////////////////////////////////////

unary_operator_prefix_parser::unary_operator_prefix_parser(precedence_t precedence) noexcept
	: _precedence(precedence)
{

}

ast_node_shared_ptr unary_operator_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto unary_operator_node = parser->ast_builder()->unary_operator_node(token);
	unary_operator_node->rhs = parser->parse_expression(r,static_cast<uint8_t>(_precedence));
	return unary_operator_node;
}

//////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr keyword_literal_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	switch (token.type) {
		case token_types_t::break_literal:
			return parser->ast_builder()->break_node(token);
		case token_types_t::continue_literal:
			return parser->ast_builder()->continue_node(token);
		case token_types_t::null_literal:
			return parser->ast_builder()->null_literal_node(token);
		case token_types_t::true_literal:
		case token_types_t::false_literal:
			return parser->ast_builder()->boolean_literal_node(token);
		default:
			return nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr number_literal_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->ast_builder()->number_literal_node(token);
}

////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr string_literal_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->ast_builder()->string_literal_node(token);
}

///////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr char_literal_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->ast_builder()->character_literal_node(token);
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr line_comment_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->ast_builder()->line_comment_node(token);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr block_comment_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return parser->ast_builder()->block_comment_node(token);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr symbol_reference_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto argument_list_node = parser->ast_builder()->argument_list_node();

	auto symbol_reference_node = parser->ast_builder()->qualified_symbol_reference_node();

	while (true) {
		auto symbol_node = parser->ast_builder()->symbol_reference_node(token);
		symbol_reference_node->lhs->children.push_back(symbol_node);
		if (!parser->peek(token_types_t::scope_operator) &&  !parser->peek(token_types_t::period)) {
			if (parser->peek(token_types_t::comma)) {
				argument_list_node->children.push_back(symbol_reference_node);
				goto next_symbol;
			}
			break;
		}
		next_symbol:
		parser->consume();
		if (!parser->expect(r, token)) {
			return nullptr;
		}
	}

	return argument_list_node->children.empty() ?
		   symbol_reference_node :
		   argument_list_node;;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr directive_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto directive_node = parser->ast_builder()->directive_node(token);
	if (parser->peek(token_types_t::semi_colon)) {
		return directive_node;
	}
	directive_node->lhs = parser->parse_expression(r, 0);
	return directive_node;
}

///////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr attribute_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto attribute_node = parser->ast_builder()->attribute_node(token);
	if (parser->peek(token_types_t::semi_colon)) {
		return attribute_node;
	}
	attribute_node->lhs = parser->parse_expression(r, 0);
	return attribute_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr array_subscript_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	ast_node_shared_ptr subscript_node = parser->ast_builder()->subscript_node();
	if (!parser->peek(token_types_t::right_square_bracket)) {
		subscript_node->lhs = parser->parse_expression(r, 0);
	}

	token_t right_bracket_token;
	right_bracket_token.type = token_types_t::right_square_bracket;
	if (!parser->expect(r, right_bracket_token)) {
		return nullptr;
	}

	return subscript_node;
}

}