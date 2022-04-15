//
// Created by 12132 on 2022/4/15.
//

#include "constant_expression_evaluator.h"

namespace gfx {
constant_expression_evaluator::constant_expression_evaluator(scope* scope) : scope_(scope)
{
}

constant_expression_evaluator::~constant_expression_evaluator()
{
}

ast_node_shared_ptr constant_expression_evaluator::evaluate(result& r, const ast_node_shared_ptr& node)
{
	if (node == nullptr) {
		return nullptr;
	}

	switch (node->type) {
		case ast_node_types_t::program:
		case ast_node_types_t::basic_block:
		case ast_node_types_t::namespace_statement: {
			for (auto const& block_child : node->children) {
				evaluate(r, block_child);
			}
			break;
		}
		case ast_node_types_t::statement: {
			if (node->lhs != nullptr) {
				evaluate(r, node->lhs);
			}

			if (node->rhs != nullptr) {
				evaluate(r, node->rhs);
			}
			break;
		}
		case ast_node_types_t::assignment: {
			break;
		}
		case ast_node_types_t::expression: {
			return evaluate(r, node->lhs);
		}
		case ast_node_types_t::unary_operator: {
			break;
		}
		case ast_node_types_t::binary_operator: {
			break;
		}
		case ast_node_types_t::symbol_reference: {
			break;
		}
		case ast_node_types_t::qualified_symbol_reference: {
			break;
		}
		default: {
			return node;
		}
	}

	return nullptr;
}
}
