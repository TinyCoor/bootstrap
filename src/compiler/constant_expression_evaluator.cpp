//
// Created by 12132 on 2022/4/15.
//

#include "constant_expression_evaluator.h"

namespace gfx {
constant_expression_evaluator::constant_expression_evaluator(scope* scope)
	: scope_(scope)
{
}

constant_expression_evaluator::~constant_expression_evaluator()
{
}

ast_node_shared_ptr constant_expression_evaluator::fold_literal_expression(result &r,
	const ast_node_shared_ptr &node)
{
	if (node == nullptr) {
		return nullptr;
	}

	switch (node->type) {
		case ast_node_types_t::expression: {
			node->rhs = fold_literal_expression(r, node->rhs);
			break;
		}
		case ast_node_types_t::statement: {
			node->rhs = fold_literal_expression(r, node->rhs);
			break;
		}
		case ast_node_types_t::assignment: {
			if (is_subtree_constant(node->rhs)) {
				auto fold_expression = fold_literal_expression(r, node->rhs);
			} else {
				return node;
			}
			break;
		}
		case ast_node_types_t::unary_operator: {
			auto folded_rhs = fold_literal_expression(r,node->rhs);
			uint64_t rhs_value ;
			if (folded_rhs->token.is_boolean()) {
				rhs_value = folded_rhs->token.as_bool() ? 1 : 0;
			} else {
				folded_rhs->token.parse(rhs_value);
			}
			switch (node->token.type) {
				case token_types_t::bang: {
					auto flag = !rhs_value;
					node->token = flag ? s_true_literal : s_false_literal;
					break;
				}
				case token_types_t::tilde: {
					node->type = ast_node_types_t::number_literal;
					node->token.type = token_types_t::number_literal;
					node->token.value = std::to_string(~rhs_value);
					break;
				}
				case token_types_t::minus: {
					node->type = ast_node_types_t::number_literal;
					node->token.type = token_types_t::number_literal;
					node->token.value = std::to_string(-rhs_value);
					break;
				}
				default:
					break;
			}
			node->lhs = nullptr;
			node->rhs = nullptr;
			break;
		}
		case ast_node_types_t::binary_operator: {
			auto folded_lhs = fold_literal_expression(r, node->lhs);
			auto folded_rhs = fold_literal_expression(r, node->rhs);

			switch (node->token.type) {
				case token_types_t::pipe:
				case token_types_t::plus:
				case token_types_t::minus:
				case token_types_t::slash:
				case token_types_t::caret:
				case token_types_t::percent:
				case token_types_t::asterisk:
				case token_types_t::ampersand:
				{
					uint64_t lhs_value, rhs_value;
					folded_lhs->token.parse(lhs_value);
					folded_rhs->token.parse(rhs_value);

					switch (node->token.type) {
						case token_types_t::plus:
							node->token.value = std::to_string(lhs_value + rhs_value);
							break;
						case token_types_t::pipe:
							node->token.value = std::to_string(lhs_value | rhs_value);
							break;
						case token_types_t::minus:
							node->token.value = std::to_string(lhs_value - rhs_value);
							break;
						case token_types_t::slash:
							node->token.value = std::to_string(lhs_value / rhs_value);
							break;
						case token_types_t::caret:
							node->token.value = std::to_string(lhs_value ^ rhs_value);
							break;
						case token_types_t::percent:
							node->token.value = std::to_string(lhs_value % rhs_value);
							break;
						case token_types_t::asterisk:
							node->token.value = std::to_string(lhs_value * rhs_value);
							break;
						case token_types_t::ampersand:
							node->token.value = std::to_string(lhs_value & rhs_value);
							break;
						default:
							break;
					}

					node->lhs = nullptr;
					node->rhs = nullptr;
					node->type = ast_node_types_t::number_literal;
					node->token.type = token_types_t::number_literal;

					break;
				}
				case token_types_t::equals: {
					node->type = ast_node_types_t::boolean_literal;
					if (*folded_lhs == *folded_rhs) {
						node->token = s_true_literal;
					} else {
						node->token = s_false_literal;
					}
					node->lhs = nullptr;
					node->rhs = nullptr;
					break;
				}
				case token_types_t::less_than: {
					break;
				}
				case token_types_t::not_equals: {
					node->type = ast_node_types_t::boolean_literal;
					if (*folded_lhs != *folded_rhs) {
						node->token = s_true_literal;
					} else {
						node->token = s_false_literal;
					}
					node->lhs = nullptr;
					node->rhs = nullptr;
					break;
				}
				case token_types_t::logical_or: {
					break;
				}
				case token_types_t::logical_and: {
					break;
				}
				case token_types_t::greater_than: {
					break;
				}
				case token_types_t::less_than_equal: {
					break;
				}
				case token_types_t::greater_than_equal: {
					break;
				}
				default: {
					break;
				}
			}
			break;
		}
		case ast_node_types_t::program:
		case ast_node_types_t::basic_block:
		case ast_node_types_t::namespace_statement: {
			for (auto it = node->children.begin(); it != node->children.end();) {
				auto block_child = *it;
				if (block_child->type == ast_node_types_t::line_comment ||
					block_child->type == ast_node_types_t::block_comment) {
					it = node->children.erase(it);
				} else {
					block_child = fold_literal_expression(r, block_child);
					++it;
				}
			}
			break;
		}
		default: {
			break;
		}
	}

	return node;
}

bool constant_expression_evaluator::is_subtree_constant(const ast_node_shared_ptr &node)
{
	if (node ==nullptr){
		return false;
	}
	switch (node->type) {
		case ast_node_types_t::expression: {
			return is_subtree_constant(node->lhs);
		}
		case ast_node_types_t::assignment: {
			return is_subtree_constant(node->rhs);
		}
		case ast_node_types_t::unary_operator: {
			return is_subtree_constant(node->rhs);
		}
		case ast_node_types_t::binary_operator: {
			return is_subtree_constant(node->lhs) && is_subtree_constant(node->rhs);
		}
		case ast_node_types_t::basic_block:
		case ast_node_types_t::line_comment:
		case ast_node_types_t::none_literal:
		case ast_node_types_t::null_literal:
		case ast_node_types_t::block_comment:
		case ast_node_types_t::empty_literal:
		case ast_node_types_t::number_literal:
		case ast_node_types_t::string_literal:
		case ast_node_types_t::boolean_literal:
		case ast_node_types_t::character_literal:
			return true;
		default:
			return false;

	}
}

ast_node_shared_ptr constant_expression_evaluator::fold_constant_symbols_and_expression(result &r,
	const ast_node_shared_ptr &node) {
	return nullptr;
}
ast_node_shared_ptr constant_expression_evaluator::fold_constant_functions_and_call_sites(result &r,
	const ast_node_shared_ptr &node) {
	return nullptr;
}
}
