//
// Created by 12132 on 2022/5/2.
//

#include "infix_parser.h"
#include "parser.h"
namespace gfx {
///////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr create_type_identifier_node(result& r, parser* parser, token_t& token)
{
	auto is_spread = false;
	ast_node_shared_ptr array_node = nullptr;

	if (parser->peek(token_types_t::left_square_bracket)) {
		array_node = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::variable));
	}

	if (parser->peek(token_types_t::spread_operator)) {
		parser->consume();
		is_spread = true;
	}

	token_t type_identifier;
	type_identifier.type = token_types_t::identifier;

	if (!parser->expect(r, type_identifier)) {
		parser->error(r, "B027", "type expected.", token.line, token.column);
		return nullptr;
	}

	auto type_node = parser->ast_builder()->type_identifier_node(type_identifier);

	if (array_node != nullptr) {
		type_node->rhs = array_node;
		type_node->flags |= ast_node_t::flags_t::array;
	}

	if (is_spread) {
		type_node->flags |= ast_node_t::flags_t::spread;
	}

	return type_node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr proc_call_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
												token_t& token)
{
	auto proc_call_node = parser->ast_builder()->proc_call_node();
	proc_call_node->lhs = lhs;

	if (!parser->peek(token_types_t::right_paren)) {
		while (true) {
			proc_call_node->rhs->children.push_back(parser->parse_expression(r, 0));
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
	return proc_call_node;
}

precedence_t proc_call_infix_parser::precedence() const
{
	return precedence_t::call;
}

///////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr symbol_reference_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	auto symbol_reference_node = parser->ast_builder()->qualified_symbol_reference_node();

	while (true) {
		auto symbol_node = parser->ast_builder()->symbol_reference_node(token);
		symbol_reference_node->lhs->children.push_back(symbol_node);
		if (!parser->peek(token_types_t::scope_operator) &&  !parser->peek(token_types_t::period)){
			break;
		}

		parser->consume();
		if (!parser->expect(r, token)) {
			return nullptr;
		}
	}

	lhs->rhs = symbol_reference_node;

	return lhs;
}

precedence_t symbol_reference_infix_parser::precedence() const
{
	return precedence_t::variable;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr type_identifier_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
														token_t& token)
{
	lhs->rhs = create_type_identifier_node(r, parser, token);
	return lhs;
}

precedence_t type_identifier_infix_parser::precedence() const
{
	return precedence_t::type;
}

///////////////////////////////////////////////////////////////////////////

binary_operator_infix_parser::binary_operator_infix_parser(precedence_t precedence,bool is_right_associative) noexcept
	: 	precedence_(precedence),
		 is_right_associative_(is_right_associative)
{
}

ast_node_shared_ptr binary_operator_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
														token_t& token)
{
	auto associative_precedence = static_cast<uint8_t>(
		static_cast<uint8_t>(precedence_) - (is_right_associative_ ? 1 : 0));
	return parser->ast_builder()->binary_operator_node(lhs, token,
													   parser->parse_expression(r, associative_precedence));
}

precedence_t binary_operator_infix_parser::precedence() const
{
	return precedence_;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr assignment_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
												   token_t& token)
{
	auto assignment_node = parser->ast_builder()->assignment_node();
	assignment_node->lhs = lhs;
	assignment_node->rhs = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::assignment) - 1);
	return assignment_node;
}

precedence_t assignment_infix_parser::precedence() const
{
	return precedence_t::assignment;
}

}