//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include "directive.h"
#include "line_comment.h"
namespace gfx::compiler {
program::program()
	: block(nullptr)
{

}

program::~program() {
	for (auto element : elements_) {
		delete element.second;
	}
	elements_.clear();
}

bool program::initialize(result& r, const ast_node_shared_ptr& root)
{
	initialize_core_types();

	if (root->type != ast_node_types_t::program) {
		r.add_message(
			"P001",
			"The root AST node must be of type 'program'.",
			true);
		return false;
	}

	evaluate(r, root);

	return true;
}

element* program::find_element(id_t id) {
	auto it = elements_.find(id);
	if (it != elements_.end())
		return it->second;
	return nullptr;
}

element* program::evaluate(result& r, const ast_node_shared_ptr& node)
{
	if (node == nullptr) {
		return nullptr;
	}
	switch (node->type) {
		case ast_node_types_t::attribute: {
			auto scope = current_scope();
			return scope->attributes().add(
				node->token.value,
				evaluate(r, node->rhs));
		}
		case ast_node_types_t::directive: {
			auto scope = current_scope();
			auto directive_element = new compiler::directive(scope, node->token.value);
			elements_.insert(std::make_pair(directive_element->id(), directive_element));
			scope->children().push_back(directive_element);
			evaluate(r, node->lhs);
			return directive_element;
		}
		case ast_node_types_t::program:
		case ast_node_types_t::basic_block: {
			make_new_block();

			if (node->type == ast_node_types_t::program) {
				initialize_core_types();
			}

			for (auto it = node->children.begin();
				 it != node->children.end();
				 ++it) {
				evaluate(r, *it);
			}

			pop_scope();

			break;
		}
		case ast_node_types_t::expression: {
			break;
		}
		case ast_node_types_t::assignment: {
			break;
		}
		case ast_node_types_t::line_comment: {
			auto scope = current_scope();
			auto comment_element = new compiler::line_comment(
				this,
				node->token.value);
			elements_.insert(std::make_pair(
				comment_element->id(),
				comment_element));
			scope->children().push_back(comment_element);
			return comment_element;
		}
		case ast_node_types_t::block_comment: {
			break;
		}
		case ast_node_types_t::unary_operator: {
			break;
		}
		case ast_node_types_t::binary_operator: {
			break;
		}
		default: {
			break;
		}
	}

	return nullptr;
}

bool program::is_subtree_constant(
	const ast_node_shared_ptr& node) {
	if (node == nullptr)
		return false;

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
			return is_subtree_constant(node->lhs)
				&& is_subtree_constant(node->rhs);
		}
		case ast_node_types_t::basic_block:
		case ast_node_types_t::line_comment:
		case ast_node_types_t::null_literal:
		case ast_node_types_t::block_comment:
		case ast_node_types_t::number_literal:
		case ast_node_types_t::string_literal:
		case ast_node_types_t::boolean_literal:
		case ast_node_types_t::character_literal:
			return true;
		default:
			return false;
	}
}

block *program::make_new_block()
{
	auto type = new block(current_scope());
	elements_.insert(std::make_pair(type->id(), type));
	push_scope(type);
	return type;
}
any_type *program::make_any_type()
{
	auto type = new any_type(current_scope());
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

string_type *program::make_string_type()
{
	auto type = new string_type(current_scope());
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

numeric_type *program::make_numeric_type(const std::string &name, int64_t min, uint64_t max)
{
	auto type = new numeric_type(current_scope(), name, min, max);
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

block *program::pop_scope()
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	auto top = scope_stack_.top();
	scope_stack_.pop();
	return top;
}

void program::initialize_core_types()
{
	auto& type_map = types();
	type_map.add("any",     make_any_type());
	type_map.add("string",  make_string_type());
	type_map.add("bool",    make_numeric_type("bool",    0,         1));
	type_map.add("u8",      make_numeric_type("u8",      0,         UINT8_MAX));
	type_map.add("u16",     make_numeric_type("u16",     0,         UINT16_MAX));
	type_map.add("u32",     make_numeric_type("u32",     0,         UINT32_MAX));
	type_map.add("u64",     make_numeric_type("u64",     0,         UINT64_MAX));
	type_map.add("s8",      make_numeric_type("s8",      INT8_MIN,  INT8_MAX));
	type_map.add("s16",     make_numeric_type("s16",     INT16_MIN, INT16_MAX));
	type_map.add("s32",     make_numeric_type("s32",     INT32_MIN, INT32_MAX));
	type_map.add("s64",     make_numeric_type("s64",     INT64_MIN, INT64_MAX));
	type_map.add("f32",     make_numeric_type("f32",     0,         UINT32_MAX));
	type_map.add("f64",     make_numeric_type("f64",     0,         UINT64_MAX));
	type_map.add("address", make_numeric_type("address", 0,         UINTPTR_MAX));
}

block *program::current_scope() const
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	return scope_stack_.top();
}

void program::push_scope(block *block)
{
	scope_stack_.push(block);
}

}