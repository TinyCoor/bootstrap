//
// Created by 12132 on 2022/5/2.
//

#include "prefix_parser.h"
#include "parser.h"
namespace gfx {

static ast_node_shared_ptr create_transmute_node(result& r, parser* parser, token_t& token)
{
    auto transmute_node = parser->ast_builder()->transmute_node(token);

    token_t less_than;
    less_than.type = token_types_t::less_than;
    if (!parser->expect(r, less_than)) {
        return nullptr;
    }

    token_t identifier;
    identifier.type = token_types_t::identifier;
    if (!parser->expect(r, identifier)) {
        return nullptr;
    }

    transmute_node->lhs = parser->ast_builder()->type_identifier_node();
    transmute_node->lhs->lhs = create_symbol_node(r, parser, nullptr, identifier);

    token_t greater_than;
    greater_than.type = token_types_t::greater_than;
    if (!parser->expect(r, greater_than)) {
        return nullptr;
    }
    token_t left_paren;
    left_paren.type = token_types_t::left_paren;
    if (!parser->expect(r, left_paren)) {
        return nullptr;
    }

    transmute_node->rhs = parser->parse_expression(r, 0);

    token_t right_paren;
    right_paren.type = token_types_t::right_paren;
    if (!parser->expect(r, right_paren)) {
        return nullptr;
    }
    return transmute_node;
}

//////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr constant_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto symbol_node = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::assignment));
	symbol_node->lhs = parser->ast_builder()->constant_node(token);
	return symbol_node;
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

ast_node_shared_ptr union_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto union_node = parser->ast_builder()->union_node(token);
	union_node->rhs = parser->parse_expression(r, 0);
	return union_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr namespace_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
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

ast_node_shared_ptr enum_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto enum_node = parser->ast_builder()->enum_node(token);
	enum_node->rhs = parser->parse_expression(r, 0);
	return enum_node;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr for_in_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto for_node = parser->ast_builder()->for_in_node(token);
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

ast_node_shared_ptr return_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto return_node = parser->ast_builder()->return_node(token);
    if (parser->peek(token_types_t::semi_colon)) {
        return return_node;
    }
	pairs_to_list(return_node->rhs, parser->parse_expression(r, 0));
	return return_node;
}

///////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr if_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto if_node = parser->ast_builder()->if_node(token);
	if_node->lhs = parser->parse_expression(r, 0);
	if_node->children.push_back(parser->parse_expression(r, 0));

	auto current_branch = if_node;
	while (true) {
		if (!parser->peek(token_types_t::else_if_literal)) {
			break;
		}
        token_t else_if_token;
        parser->current(else_if_token);
		parser->consume();
		current_branch->rhs = parser->ast_builder()->else_if_node(else_if_token);
		current_branch->rhs->lhs = parser->parse_expression(r, 0);
		current_branch->rhs->children.push_back(parser->parse_expression(r, 0));
		current_branch = current_branch->rhs;
	}

	if (parser->peek(token_types_t::else_literal)) {
        token_t else_token;
        parser->current(else_token);
		parser->consume();
		current_branch->rhs = parser->ast_builder()->else_node(else_token);
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
	return parser->parse_scope(r, token);
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr proc_expression_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto proc_node = parser->ast_builder()->proc_expression_node(token);

	token_t left_paren_token;
	left_paren_token.type = token_types_t::left_paren;
	if (!parser->expect(r, left_paren_token)){
		return nullptr;
	}

	if (!parser->peek(token_types_t::right_paren)) {
		pairs_to_list(proc_node->rhs, parser->parse_expression(r, 0));
	}

	token_t right_paren_token;
	right_paren_token.type = token_types_t::right_paren;
	if (!parser->expect(r, right_paren_token)) {
		return nullptr;
	}

	if (parser->peek(token_types_t::colon)) {
		parser->consume();
		pairs_to_list(proc_node->lhs, parser->parse_expression(r, 0));
	}

	if (!parser->peek(token_types_t::semi_colon)) {
		proc_node->children.push_back(parser->parse_expression(r, 0));
	}

	return proc_node;
}

/////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr group_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return create_expression_node(r, parser, nullptr, token);
}

///////////////////////////////////////////////////////////////////////////////////////////

unary_operator_prefix_parser::unary_operator_prefix_parser(precedence_t precedence) noexcept
	: precedence_(precedence)
{

}

ast_node_shared_ptr unary_operator_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto unary_operator_node = parser->ast_builder()->unary_operator_node(token);
	auto rhs = parser->parse_expression(r,static_cast<uint8_t>(precedence_));
	if (rhs == nullptr) {
		parser->error(r, "P019", "unary operator expects right-hand-side expression", token.location);
		return nullptr;
	}
	unary_operator_node->rhs = rhs;
	unary_operator_node->location.start(token.location.start());
	unary_operator_node->location.end(unary_operator_node->rhs->location.end());
	return unary_operator_node;
}

//////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr keyword_literal_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	switch (token.type) {
		case token_types_t::cast_literal: {
			auto cast_node = parser->ast_builder()->cast_node(token);
			return cast_node;
		}
		case token_types_t::import_literal: {
			auto import_node = parser->ast_builder()->import_node(token);
			import_node->lhs = parser->parse_expression(r, 0);
			if (import_node->lhs == nullptr) {
				parser->error(r, "P019", "import expects namespace", token.location);
				return nullptr;
			}
            if (parser->peek(token_types_t::from_literal)) {
				token_t from_token;
				parser->current(from_token);
                parser->consume();
                import_node->rhs = parser->parse_expression(r, 0);
				if (import_node->rhs == nullptr) {
					parser->error(r, "P019", "from expects identifier of type module", from_token.location);
					return nullptr;
				}
            }
			return import_node;
		}
		case token_types_t::alias_literal: {
			auto alias_node = parser->ast_builder()->alias_node(token);
			alias_node->lhs = parser->expect_expression(r, ast_node_types_t::assignment, 0);
			return alias_node;
		}
		case token_types_t::break_literal: {
			return parser->ast_builder()->break_node(token);
		}
		case token_types_t::continue_literal: {
			return parser->ast_builder()->continue_node(token);
		}
		case token_types_t::null_literal: {
			return parser->ast_builder()->null_literal_node(token);
		}
		case token_types_t::true_literal:
		case token_types_t::false_literal: {
			return parser->ast_builder()->boolean_literal_node(token);
		}
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

ast_node_shared_ptr symbol_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return create_symbol_node(r, parser, nullptr, token);
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

/////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr attribute_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	auto attribute_node = parser->ast_builder()->attribute_node(token);
	if (parser->peek(token_types_t::semi_colon)) {
		return attribute_node;
	}
	attribute_node->lhs = parser->parse_expression(r, 0);
	return attribute_node;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

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

ast_node_shared_ptr label_prefix_parser::parse(result& r, parser* parser,token_t& token)
{
	return parser->ast_builder()->label_node(token);
}

/////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr cast_prefix_parser::parse(result& r, parser* parser, token_t& token)
{
	return create_cast_node(r, parser, token);
}


ast_node_shared_ptr from_prefix_parser::parse(result &r, parser *parser, token_t &token)
{
    auto from_node = parser->ast_builder()->from_node(token);
    from_node->rhs = parser->parse_expression(r, 0);
    return from_node;
}

ast_node_shared_ptr module_prefix_parser::parse(result &r, parser *parser, token_t &token)
{
    return create_module_expression_node(r, parser, nullptr, token);

}

ast_node_shared_ptr transmute_prefix_parser::parse(result &r, parser *parser, token_t &token)
{
    return create_transmute_node(r, parser, token);
}
}