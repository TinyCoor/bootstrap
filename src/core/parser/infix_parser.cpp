//
// Created by 12132 on 2022/5/2.
//

#include "infix_parser.h"
#include "parser.h"
namespace gfx {

static ast_node_shared_ptr create_assignment_node(result& r, ast_node_types_t type, parser* parser,
    const ast_node_shared_ptr& lhs, token_t& token)
{
    ast_node_shared_ptr assignment_node;
    if (type == ast_node_types_t::assignment) {
        assignment_node = parser->ast_builder()->assignment_node();
    } else if (type == ast_node_types_t::constant_assignment) {
        assignment_node = parser->ast_builder()->constant_assignment_node();
    }

    pairs_to_list(assignment_node->lhs, lhs);
    auto rhs = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::assignment));
    if (rhs == nullptr) {
        parser->error(r, "P019", "assignment expects right-hand-side expression", token.location);
        return nullptr;
    }
    pairs_to_list(assignment_node->rhs, rhs);

    assignment_node->location.start(lhs->location.start());
    assignment_node->location.end(assignment_node->rhs->location.end());

    return assignment_node;
}
ast_node_shared_ptr create_module_expression_node(result& r, parser* parser,
     const ast_node_shared_ptr& lhs, token_t& token) {
    auto module_expression_node = parser->ast_builder()->module_expression_node(token);
    token_t left_paren;
    left_paren.type = token_types_t::left_paren;
    if (!parser->expect(r, left_paren)) {
        return nullptr;
    }
    module_expression_node->rhs = parser->parse_expression(r, 0);

    token_t right_paren;
    right_paren.type = token_types_t::right_paren;
    if (!parser->expect(r, right_paren)) {
        return nullptr;
    }
    return module_expression_node;
}

/////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr create_type_identifier_node(result& r, parser* parser, token_t& token)
{
	auto is_spread = false;
    auto is_pointer = false;
	ast_node_shared_ptr array_node = nullptr;
    auto array_subscripts = parser->ast_builder()->array_subscript_list_node();
    while (true) {
        if (!parser->peek(token_types_t::left_square_bracket)) {
            break;
        }
        array_subscripts->children.push_back(parser->parse_expression(r,
            static_cast<uint8_t>(precedence_t::variable)));
    }

    if (parser->peek(token_types_t::caret)) {
        parser->consume();
        is_pointer = true;
    }

	if (parser->peek(token_types_t::spread_operator)) {
		parser->consume();
		is_spread = true;
	}

	token_t type_identifier;
	type_identifier.type = token_types_t::identifier;

	if (!parser->expect(r, type_identifier)) {
		parser->error(r, "B027", "type expected.", token.location);
		return nullptr;
	}

    auto symbol_node = create_symbol_node(r, parser, nullptr, type_identifier);
    auto type_node = parser->ast_builder()->type_identifier_node();
    type_node->lhs = symbol_node;
    type_node->rhs = array_subscripts;
    if (!array_subscripts->children.empty()) {
        type_node->flags |= ast_node_t::flags_t::array;
    }
	if (is_spread) {
		type_node->flags |= ast_node_t::flags_t::spread;
	}

    if (is_pointer) {
        type_node->flags |= ast_node_t::flags_t::pointer;
    }
    return type_node;
}

void pairs_to_list(const ast_node_shared_ptr& target, const ast_node_shared_ptr& root)
{
    if (root == nullptr) {
        return;
    }
	if (root->type != ast_node_types_t::pair) {
        target->location = root->location;
		target->children.push_back(root);
		return;
	}

	auto current_pair = root;
    target->location.start(current_pair->location.start());
	while (true) {
		if (current_pair->lhs->type != ast_node_types_t::pair) {
			target->children.push_back(current_pair->lhs);
			target->children.push_back(current_pair->rhs);
            target->location.end(current_pair->rhs->location.end());
			break;
		}
		target->children.push_back(current_pair->rhs);
		current_pair = current_pair->lhs;
	}
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr create_expression_node(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	auto expression_node = parser->ast_builder()->expression_node();
	expression_node->lhs = parser->parse_expression(r, 0);

	token_t right_paren_token;
	right_paren_token.type = token_types_t::right_paren;
	if (!parser->expect(r, right_paren_token))  {
		return nullptr;
	}

	if (lhs != nullptr
		&&  lhs->type == ast_node_types_t::block_comment) {
		expression_node->children.push_back(lhs);
	}

	return expression_node;
}

ast_node_shared_ptr create_symbol_node(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	auto symbol_node = parser->ast_builder()->symbol_node();
    symbol_node->location.start(token.location.start());

	while (true) {
		auto symbol_part_node = parser->ast_builder()->symbol_part_node(token);
		symbol_node->children.push_back(symbol_part_node);
		if (!parser->peek(token_types_t::scope_operator)) {
            symbol_node->location.end(token.location.end());
            break;
		}
		parser->consume();
		if (!parser->expect(r, token)) {
			return nullptr;
		}
	}

	if (lhs != nullptr
		&&  (lhs->token.is_block_comment() || lhs->token.is_line_comment())) {
		symbol_node->children.push_back(lhs);
	}

	return symbol_node;
}

ast_node_shared_ptr create_cast_node(result& r, parser* parser, token_t& token)
{
	auto cast_node = parser->ast_builder()->cast_node(token);

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

	cast_node->lhs = parser->ast_builder()->type_identifier_node();
    cast_node->lhs->lhs = create_symbol_node(r, parser, nullptr, identifier);

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

	cast_node->rhs = parser->parse_expression(r, 0);

	token_t right_paren;
	right_paren.type = token_types_t::right_paren;
	if (!parser->expect(r, right_paren)) {
		return nullptr;
	}

	return cast_node;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr proc_call_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	if (lhs->type == ast_node_types_t::symbol) {
		auto proc_call_node = parser->ast_builder()->proc_call_node();
		proc_call_node->lhs = lhs;
        proc_call_node->location.start(lhs->location.start());

        if (!parser->peek(token_types_t::right_paren)) {
            pairs_to_list(proc_call_node->rhs, parser->parse_expression(r, 0));
		}
		token_t right_paren_token;
		right_paren_token.type = token_types_t::right_paren;
		if (!parser->expect(r, right_paren_token)) {
			return nullptr;
		}
        proc_call_node->location.end(right_paren_token.location.end());
        return proc_call_node;
	} else {
		return create_expression_node(r, parser, lhs, token);
	}
}

precedence_t proc_call_infix_parser::precedence() const
{
	return precedence_t::call;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr pointer_dereference_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
    auto node = parser->ast_builder()->unary_operator_node(token);
    node->rhs = lhs;
    return node;
}

precedence_t pointer_dereference_infix_parser::precedence() const
{
	return precedence_t::pointer_dereference;
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

binary_operator_infix_parser::binary_operator_infix_parser(precedence_t precedence, bool is_right_associative) noexcept
	: 	precedence_(precedence), is_right_associative_(is_right_associative)
{
}

ast_node_shared_ptr binary_operator_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	auto associative_precedence = static_cast<uint8_t>(
		static_cast<uint8_t>(precedence_) - (is_right_associative_ ? 1 : 0));
    auto rhs = parser->parse_expression(r, associative_precedence);
    if (rhs == nullptr) {
        parser->error(r, "P019", "binary operator expects right-hand-side expression",token.location);
        return nullptr;
    }
	return parser->ast_builder()->binary_operator_node(lhs, token, rhs);
}

precedence_t binary_operator_infix_parser::precedence() const
{
	return precedence_;
}

///////////////////////////////////////////////////////////////////////////

ast_node_shared_ptr assignment_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
    return create_assignment_node(r, ast_node_types_t::assignment, parser, lhs, token);
}

precedence_t assignment_infix_parser::precedence() const
{
	return precedence_t::assignment;
}

ast_node_shared_ptr block_comment_infix_parser::parse(result &r,parser *parser, const ast_node_shared_ptr &lhs,
	token_t &token)
{
	auto block_comment_node = parser->ast_builder()->block_comment_node(token);
	lhs->children.push_back(block_comment_node);
	return lhs;
}

precedence_t block_comment_infix_parser::precedence() const
{
	return precedence_t::block_comment;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
ast_node_shared_ptr cast_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs, token_t& token)
{
	lhs->rhs = create_cast_node(r, parser, token);
	return lhs;
}

precedence_t cast_infix_parser::precedence() const
{
	return precedence_t::cast;
}

ast_node_shared_ptr comma_infix_parser::parse(result& r, parser* parser, const ast_node_shared_ptr& lhs,
	token_t& token)
{
	auto pair_node = parser->ast_builder()->pair_node();
	pair_node->lhs = lhs;
	pair_node->rhs = parser->parse_expression(r, static_cast<uint8_t>(precedence_t::comma));
	return pair_node;
}

precedence_t comma_infix_parser::precedence() const
{
	return precedence_t::comma;
}
ast_node_shared_ptr constant_assignment_infix_parser::parse(result &r, parser *parser,
     const ast_node_shared_ptr &lhs, token_t &token)
{
    return create_assignment_node(r, ast_node_types_t::constant_assignment,
        parser, lhs, token);
}

precedence_t constant_assignment_infix_parser::precedence() const
{
    return precedence_t::assignment;
}
}