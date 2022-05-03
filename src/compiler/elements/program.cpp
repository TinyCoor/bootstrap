//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include "attribute.h"
#include "directive.h"
#include "line_comment.h"
#include "numeric_type.h"
#include "string_type.h"
#include "any_type.h"
#include "composite_type.h"
#include "binary_operator.h"
#include "fmt/format.h"
#include "block_comment.h"
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
			auto attribute = make_attribute(node->token.value, evaluate(r, node->rhs));
			scope->attributes().add(attribute);
			return attribute;
		}
		case ast_node_types_t::directive: {
			auto scope = current_scope();
			auto directive_element = make_directive(node->token.value);
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
		case ast_node_types_t::statement:
		case ast_node_types_t::expression: {
			break;
		}
		case ast_node_types_t::assignment: {
			// binary_operator returned from here
//                auto list_of_identifiers = evaluate(r, node->lhs);
//                auto list_of_expressions = evaluate(r, node->rhs);
			break;
		}
		case ast_node_types_t::line_comment: {
			auto scope = current_scope();
			auto comment = make_line_comment(node->token.value);
			scope->children().push_back(comment);
			return comment;
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
		case ast_node_types_t::proc_expression: {
			break;
		}
		case ast_node_types_t::enum_expression: {
			auto scope = current_scope();
			auto& scope_types = scope->types();

			auto type = make_enum();
			scope_types.add(type);

			for (const auto& child : node->rhs->children) {
				auto field_element = evaluate(r, child);
				auto field = make_field("", nullptr, nullptr);
				type->fields().add(field);
			}

			return type;
		}
		case ast_node_types_t::union_expression: {
			auto scope = current_scope();
			auto& scope_types = scope->types();

			auto type = make_union();
			scope_types.add(type);

			for (const auto& child : node->rhs->children) {
			}

			return type;
		}
		case ast_node_types_t::struct_expression: {
			auto scope = current_scope();
			auto& scope_types = scope->types();

			auto type = make_struct();
			scope_types.add(type);

			for (const auto& child : node->rhs->children) {
			}

			return type;
		}
		case ast_node_types_t::constant_expression: {
			// XXX: highly doubt this correct
			auto assignment = dynamic_cast<binary_operator*>(evaluate(r, node->rhs));
			auto variable = dynamic_cast<identifier*>(assignment->lhs());
			variable->constant(true);
			break;
		}
		case ast_node_types_t::qualified_symbol_reference: {
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
	auto& scope_types = current_scope()->types();
	scope_types.add(make_any_type());
	scope_types.add(make_string_type());
	scope_types.add(make_numeric_type("bool",    0,         1));
	scope_types.add(make_numeric_type("u8",      0,         UINT8_MAX));
	scope_types.add(make_numeric_type("u16",     0,         UINT16_MAX));
	scope_types.add(make_numeric_type("u32",     0,         UINT32_MAX));
	scope_types.add(make_numeric_type("u64",     0,         UINT64_MAX));
	scope_types.add(make_numeric_type("s8",      INT8_MIN,  INT8_MAX));
	scope_types.add(make_numeric_type("s16",     INT16_MIN, INT16_MAX));
	scope_types.add(make_numeric_type("s32",     INT32_MIN, INT32_MAX));
	scope_types.add(make_numeric_type("s64",     INT64_MIN, INT64_MAX));
	scope_types.add(make_numeric_type("f32",     0,         UINT32_MAX));
	scope_types.add(make_numeric_type("f64",     0,         UINT64_MAX));
	scope_types.add(make_numeric_type("address", 0,         UINTPTR_MAX));
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

field* program::make_field(const std::string& name, compiler::type* type, compiler::initializer* initializer)
{
	auto field = new compiler::field(current_scope(), name, type, initializer);
	elements_.insert(std::make_pair(field->id(), field));
	return field;
}

attribute *program::make_attribute(const std::string &name, element *expr)
{
	auto attr = new attribute(current_scope(), name, expr);
	elements_.insert(std::make_pair(attr->id(), attr));
	return attr;
}

composite_type* program::make_enum() {
	auto type = new composite_type(current_scope(), composite_types_t::enum_type,
		fmt::format("__enum_{}__", id_pool::instance()->allocate()));
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

composite_type* program::make_union() {
	auto type = new composite_type(current_scope(), composite_types_t::union_type,
		fmt::format("__union_{}__", id_pool::instance()->allocate()));
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

composite_type* program::make_struct() {
	auto type = new composite_type(current_scope(), composite_types_t::struct_type,
		fmt::format("__struct_{}__", id_pool::instance()->allocate()));
	elements_.insert(std::make_pair(type->id(), type));
	return type;
}

identifier *program::make_identifier(element *expr)
{
	return nullptr;
}

directive *program::make_directive(const std::string &name)
{
	auto directive1 = new compiler::directive(current_scope(), name);
	elements_.insert(std::make_pair(directive1->id(), directive1));
	return directive1;
}

line_comment *program::make_line_comment(const std::string &value)
{
	auto comment = new compiler::line_comment(current_scope(), value);
	elements_.insert(std::make_pair(comment->id(), comment));
	return comment;
}

block_comment *program::make_block_comment(const std::string &value)
{
	auto comment = new compiler::block_comment(current_scope(), value);
	elements_.insert(std::make_pair(comment->id(), comment));
	return comment;
}

}