//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include "cast.h"
#include "label.h"
#include "alias.h"
#include "type.h"
#include "import.h"
#include "comment.h"
#include "any_type.h"
#include "tuple_type.h"
#include "array_type.h"
#include "if_element.h"
#include "expression.h"
#include "attribute.h"
#include "directive.h"
#include "statement.h"
#include "type_info.h"
#include "string_type.h"
#include "numeric_type.h"
#include "procedure_type.h"
#include "unknown_type.h"
#include "argument_list.h"
#include "boolean_literal.h"
#include "string_literal.h"
#include "float_literal.h"
#include "integer_literal.h"
#include "return_element.h"
#include "unary_operator.h"
#include "composite_type.h"
#include "procedure_call.h"
#include "binary_operator.h"
#include "namespace_element.h"
#include "namespace_type.h"
#include "procedure_instance.h"
#include "fmt/format.h"
#include <array>
namespace gfx::compiler {

program::program(class terp* terp)
	: element(nullptr,  element_type_t::program), assembler_(terp), terp_(terp)
{

}

program::~program()
{
	for (auto& element : elements_) {
		delete element.second;
	}
	elements_.clear();
}

compiler::block* program::block()
{
	return block_;
}

element* program::find_element(id_t id)
{
	auto it = elements_.find(id);
	if (it != elements_.end()) {
		return it->second;
	}
	return nullptr;
}

element* program::evaluate(result& r, const ast_node_shared_ptr& node, element_type_t default_block_type)
{
	if (node == nullptr) {
		return nullptr;
	}
	switch (node->type) {
		case ast_node_types_t::symbol: {
			if (node->has_type_identifier()) {
				return add_identifier_to_scope(r, node, nullptr);
			} else {
				return find_identifier(node);
			}
		}
		case ast_node_types_t::attribute: {
			return make_attribute(current_scope(), node->token.value, evaluate(r, node->lhs));
		}
		case ast_node_types_t::directive: {
			auto expression = evaluate(r, node->lhs);
			auto directive_element = make_directive(current_scope(), node->token.value, expression);
			apply_attributes(r, directive_element, node);
			directive_element->evaluate(r, this);
			return directive_element;
		}
		case ast_node_types_t::module: {
			for (auto &it : node->children) {
				add_expression_to_scope(block_, evaluate(r, it));
			}
			return block_;
		}
		case ast_node_types_t::basic_block: {
			auto active_block = push_new_block(default_block_type);
			for (auto &it : node->children) {
				add_expression_to_scope(active_block, evaluate(r, it, default_block_type));
			}
			return pop_scope();
		}
		case ast_node_types_t::statement: {
			label_list_t labels{};
			if (node->lhs != nullptr) {
				for (const auto & label: node->lhs->children) {
					labels.push_back(make_label(current_scope(), label->token.value));
				}
			}
			return make_statement(current_scope(), labels, evaluate(r, node->rhs));
		}
		case ast_node_types_t::expression: {
			return make_expression(current_scope(), evaluate(r, node->lhs));
		}
		case ast_node_types_t::assignment: {
			const auto &assignment_target_list = node->lhs;

			identifier_list_t list {};
			for (const auto& symbol : assignment_target_list->children) {
				auto existing_identifier = find_identifier(symbol);
				if (existing_identifier != nullptr) {
					return make_binary_operator(current_scope(), operator_type_t::assignment, existing_identifier,
						evaluate(r, node->rhs));
				} else {
					auto new_identifier = add_identifier_to_scope(r, symbol, node->rhs);
					list.push_back(new_identifier);
				}
			}
			// TODO: handle proper multi-assignment

			return list.front();
		}
		case ast_node_types_t::line_comment: {
			return make_comment(current_scope(), comment_type_t::line, node->token.value);
		}
		case ast_node_types_t::block_comment: {
			return make_comment(current_scope(), comment_type_t::block, node->token.value);
		}
		case ast_node_types_t::string_literal: {
			return make_string(current_scope(), node->token.value);
		}
		case ast_node_types_t::number_literal: {
			switch (node->token.number_type) {
				case number_types_t::integer: {
					uint64_t value;
					if (node->token.parse(value) == conversion_result_t::success) {
						return make_integer(current_scope(), value);
					}
					// TODO: need to handle conversion failures
				}
				case number_types_t::floating_point: {
					double value;
					if (node->token.parse(value) == conversion_result_t::success) {
						return make_float(current_scope(), value);
					}
					// TODO: need to handle conversion failures
				}
				default:
					break;
			}
		}
		case ast_node_types_t::boolean_literal: {
			return make_bool(current_scope(), node->token.as_bool());
		}
		case ast_node_types_t::else_expression: {
			return evaluate(r, node->children[0]);
		}
		case ast_node_types_t::import_expression: {
			return make_import(current_scope(), evaluate(r, node->lhs));
		}
		case ast_node_types_t::if_expression:
		case ast_node_types_t::elseif_expression: {
			auto predicate = evaluate(r, node->lhs);
			auto true_branch = evaluate(r, node->children[0]);
			auto false_branch = evaluate(r, node->rhs);
			return make_if(current_scope(), predicate, true_branch, false_branch);
		}
		case ast_node_types_t::unary_operator: {
			auto it = s_unary_operators.find(node->token.type);
			if (it == s_unary_operators.end()) {
				return nullptr;
			}
			return make_unary_operator(current_scope(), it->second, evaluate(r, node->rhs));
		}
		case ast_node_types_t::binary_operator: {
			auto it = s_binary_operators.find(node->token.type);
			if (it == s_binary_operators.end()) {
				return nullptr;
			}
            auto lhs = evaluate(r, node->lhs);
            auto rhs = evaluate(r, node->rhs);
			return make_binary_operator(current_scope(), it->second, lhs, rhs);
		}
		case ast_node_types_t::argument_list: {
			auto args = make_argument_list(current_scope());
			for (const auto& arg : node->children) {
				args->add(evaluate(r, arg));
			}
			return args;
		}
		case ast_node_types_t::proc_call: {
			auto proc_identifier = find_identifier(node->lhs);
			if (proc_identifier != nullptr) {
				auto args = (argument_list*)nullptr;
				auto expr = evaluate(r, node->rhs);
				if (expr != nullptr) {
					args = dynamic_cast<argument_list*>(expr);
				}
				return make_procedure_call(current_scope(), proc_identifier, args);
			}
			return nullptr;
		}
		case ast_node_types_t::proc_expression: {
			auto active_scope = current_scope();
			auto block_scope = make_block(active_scope, element_type_t::proc_type_block);
			auto proc_type = make_procedure_type(r, active_scope, block_scope);
			current_scope()->types().add(proc_type);

			auto count = 0;
			for (const auto& type_node : node->lhs->children) {
				switch (type_node->type) {
					case ast_node_types_t::symbol: {
						auto return_identifier = make_identifier(block_scope, fmt::format("_{}", count++), nullptr);
                        return_identifier->usage(identifier_usage_t::stack);
                        return_identifier->type(find_type(type_node->children[0]->token.value));
						proc_type->returns().add(make_field(block_scope, return_identifier));
						break;
					}
					default: {
						break;
					}
				}
			}

			for (const auto& param_node : node->rhs->children) {
				switch (param_node->type) {
					case ast_node_types_t::assignment: {
						// XXX: in the parameter list, multiple targets is an error
						const auto& symbol_node = param_node->lhs->children[0];
						auto param_identifier = add_identifier_to_scope(r, symbol_node, param_node->rhs, block_scope);
                        param_identifier->usage(identifier_usage_t::stack);
                        auto field = make_field(current_scope(), param_identifier);
						proc_type->parameters().add(field);
						break;
					}
					case ast_node_types_t ::symbol: {
						auto param_identifier = add_identifier_to_scope(r, param_node, nullptr, block_scope);
                        param_identifier->usage(identifier_usage_t::stack);
                        auto field = make_field(block_scope, param_identifier);
						proc_type->parameters().add(field);
						break;
					}
					default: {
						break;
					}
				}
			}

			return proc_type;
		}
		case ast_node_types_t::enum_expression: {
			auto active_scope =current_scope();
			auto enum_type = make_enum_type(r, active_scope);
            current_scope()->types().add(enum_type);
            add_composite_type_fields(r, enum_type, node->rhs);
            if (!enum_type->initialize(r, this)) {
                return nullptr;
            }
			return enum_type;
		}
		case ast_node_types_t::cast_expression: {
			auto type = find_type(node->lhs->token.value);
			if (type == nullptr) {
				r.add_message("P002", fmt::format("unknown type '{}'.", node->lhs->token.value), true);
			}
			return make_cast(current_scope(), type, evaluate(r, node->rhs));
		}
		case ast_node_types_t::alias_expression: {
			return make_alias(current_scope(), evaluate(r, node->lhs));
		}
		case ast_node_types_t::union_expression: {
			auto union_type = make_union_type(r, current_scope());
            current_scope()->types().add(union_type);
            add_composite_type_fields(r, union_type, node->rhs);
            if (!union_type->initialize(r, this)) {
                return nullptr;
            }
			return union_type;
		}
		case ast_node_types_t::struct_expression: {
			auto struct_type = make_struct_type(r, current_scope());
            current_scope()->types().add(struct_type);
            add_composite_type_fields(r, struct_type, node->rhs);
            if (!struct_type->initialize(r, this)) {
                return nullptr;
            }
			return struct_type;
		}
		case ast_node_types_t::return_statement: {
			auto return_element = make_return(current_scope());
			for (const auto& arg_node : node->rhs->children) {
				return_element->expressions().push_back(evaluate(r, arg_node));
			}
			return return_element;
		}
		case ast_node_types_t::constant_expression: {
			auto identifier = dynamic_cast<class identifier*>(evaluate(r, node->rhs));
			if (identifier !=nullptr) {
				identifier->constant(true);
			}
			return identifier;
		}
		case ast_node_types_t::namespace_expression: {
			return make_namespace(current_scope(), evaluate(r, node->rhs));
		}
		default: {
			break;
		}
	}

	return nullptr;
}

block *program::push_new_block(element_type_t type)
{
	auto parent_scope = current_scope();
	auto scope_block = make_block(parent_scope, type);

	if (parent_scope != nullptr) {
		parent_scope->blocks().push_back(scope_block);
	}

	push_scope(scope_block);
	return scope_block;
}

any_type *program::make_any_type(result &r, compiler::block* parent_scope)
{
    return make_type<any_type>(r, parent_scope);
}

string_type *program::make_string_type(result &r, compiler::block* parent_scope)
{
    return make_type<string_type>(r, parent_scope);
}

numeric_type *program::make_numeric_type(result &r, compiler::block* parent_scope, const std::string &name, int64_t min, uint64_t max)
{
    return make_type<numeric_type>(r, parent_scope, name, min, max);
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

void program::initialize_core_types(result &r)
{
	auto parent_scope = current_scope();
    auto numeric_types = numeric_type::make_types(r, parent_scope, this);

    auto string_type = make_string_type(r, parent_scope);
    string_type->initialize(r, this);
	add_type_to_scope(string_type);

    auto namespace_type = make_namespace_type(r, parent_scope);
    namespace_type->initialize(r, this);
    add_type_to_scope(namespace_type);

    auto type_info_type = make_type_info_type(r, parent_scope);
    type_info_type->initialize(r, this);
    add_type_to_scope(type_info_type);

    auto tuple_type = make_type_info_type(r, parent_scope);
    tuple_type->initialize(r, this);
    add_type_to_scope(tuple_type);

    auto any_type = make_any_type(r, parent_scope);
    any_type->initialize(r, this);
    add_type_to_scope(any_type);
}

block *program::current_scope() const
{
	if (scope_stack_.empty()) {
		return nullptr;
	}
	return scope_stack_.top();
}

void program::push_scope(class block *block)
{
	scope_stack_.push(block);
}

field* program::make_field(compiler::block* parent_scope, compiler::identifier* identifier)
{
    auto field = new compiler::field(parent_scope, identifier);
    identifier->parent_element(field);
    elements_.insert(std::make_pair(field->id(), field));
    return field;
}

attribute *program::make_attribute(compiler::block* parent_scope, const std::string &name, element *expr)
{
    auto attr = new compiler::attribute(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(attr);
    }
    elements_.insert(std::make_pair(attr->id(), attr));
    return attr;
}

composite_type* program::make_enum_type(result &r, compiler::block* parent_block)
{
    std::string name = fmt::format("__enum_{}__", id_pool::instance()->allocate());
    return make_type<composite_type>(r, parent_block, composite_types_t::enum_type, name);
}

composite_type* program::make_union_type(result& r, compiler::block* parent_block)
{
    std::string name = fmt::format("__union_{}__", id_pool::instance()->allocate());
    return make_type<composite_type>(r, parent_block, composite_types_t::union_type, name);
}

composite_type* program::make_struct_type(result& r, compiler::block* parent_scope)
{
    std::string name = fmt::format("__struct_{}__", id_pool::instance()->allocate());
    return make_type<composite_type>(r, parent_scope, composite_types_t::struct_type, name);
}

comment *program::make_comment(compiler::block* parent_scope, comment_type_t type, const std::string &value)
{
    auto commnet = new compiler::comment(parent_scope, type, value);
    elements_.insert(std::make_pair(commnet->id(), commnet));
    return commnet;
}

directive *program::make_directive(compiler::block* parent_scope, const std::string &name, element *expr)
{
    auto directive = new compiler::directive(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(directive);
    }
    elements_.insert(std::make_pair(directive->id(), directive));
    return directive;
}

statement *program::make_statement(compiler::block* parent_scope, const label_list_t &labels, element *expr)
{
	auto statement = new compiler::statement(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(statement);
    }

	for (auto &label : labels) {
		statement->labels().push_back(label);
        label->parent_element(statement);
	}
	elements_.insert(std::make_pair(statement->id(), statement));
	return statement;
}

label *program::make_label(compiler::block* parent_scope, const std::string &name)
{
    auto label = new compiler::label(parent_scope, name);
    elements_.insert(std::make_pair(label->id(), label));
    return label;
}

identifier* program::make_identifier(compiler::block* parent_scope, const std::string& name, initializer* expr)
{
    auto identifier = new compiler::identifier(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(identifier);
    }
    elements_.insert(std::make_pair(identifier->id(), identifier));
    return identifier;
}

unary_operator* program::make_unary_operator(compiler::block* parent_scope, operator_type_t type, element* rhs)
{
    auto unary_operator = new compiler::unary_operator(parent_scope, type, rhs);
    rhs->parent_element(unary_operator);
    elements_.insert(std::make_pair(unary_operator->id(), unary_operator));
    return unary_operator;
}

binary_operator* program::make_binary_operator(compiler::block* parent_scope, operator_type_t type, element* lhs, element* rhs)
{
    auto element = new binary_operator(parent_scope, type, lhs, rhs);
    lhs->parent_element(element);
    rhs->parent_element(element);
    elements_.insert(std::make_pair(element->id(), element));
    return element;
}

expression* program::make_expression(compiler::block* parent_scope, element* expr)
{
    auto expression = new compiler::expression(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(expression);
    }
    elements_.insert(std::make_pair(expression->id(), expression));
    return expression;
}

alias *program::make_alias(compiler::block* parent_scope, element *expr)
{
    auto alias_type = new compiler::alias(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(alias_type);
    }
    elements_.insert(std::make_pair(alias_type->id(), alias_type));
    return alias_type;
}

procedure_type* program::make_procedure_type(result &r, compiler::block* parent_scope, compiler::block* block_scope)
{
	// XXX: the name of the proc isn't correct here, but it works temporarily.
    std::string name = fmt::format("__proc_{}__", id_pool::instance()->allocate());
    auto type = new procedure_type(parent_scope, block_scope, name);
    if (block_scope != nullptr) {
        block_scope->parent_element(type);
    }
    elements_.insert(std::make_pair(type->id(), type));
    return type;
}

procedure_instance* program::make_procedure_instance(compiler::block* parent_scope,
	compiler::type* procedure_type, compiler::block* scope)
{
    auto instance = new compiler::procedure_instance(parent_scope, procedure_type, scope);
    scope->parent_element(instance);
    elements_.insert(std::make_pair(instance->id(), instance));
    return instance;
}

initializer* program::make_initializer(compiler::block* parent_scope, element* expr)
{
    auto initializer = new compiler::initializer(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(initializer);
    }
    elements_.insert(std::make_pair(initializer->id(), initializer));
    return initializer;
}

cast* program::make_cast(compiler::block* parent_scope, compiler::type* type, element* expr)
{
    auto cast = new compiler::cast(parent_scope, type, expr);
    elements_.insert(std::make_pair(cast->id(), cast));
    return cast;
}

if_element *program::make_if(compiler::block* parent_scope, element *predicate,
	element *true_branch, element *false_branch)
{
    auto if_element = new compiler::if_element(parent_scope, predicate, true_branch, false_branch);
    if (predicate != nullptr) {
        predicate->parent_element(if_element);
    }
    if (true_branch != nullptr) {
        true_branch->parent_element(if_element);
    }
    if (false_branch != nullptr) {
        false_branch->parent_element(if_element);
    }
    elements_.insert(std::make_pair(if_element->id(), if_element));
    return if_element;
}

return_element *program::make_return(compiler::block* parent_scope)
{
    auto return_element = new compiler::return_element(parent_scope);
    elements_.insert(std::make_pair(return_element->id(), return_element));
    return return_element;
}

argument_list* program::make_argument_list(compiler::block* parent_scope)
{
    auto arg = new compiler::argument_list(parent_scope);
    elements_.insert(std::make_pair(arg->id(), arg));
    return arg;
}

procedure_call *program::make_procedure_call(compiler::block* parent_scope, compiler::identifier* identifier,
	compiler::argument_list* args)
{
    auto call = new compiler::procedure_call(parent_scope, identifier, args);
    elements_.insert(std::make_pair(call->id(), call));
    return call;
}

namespace_element *program::make_namespace(compiler::block* parent_scope, element *expr, const std::string &name)
{
    auto ns = new compiler::namespace_element(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(ns);
    }
    elements_.insert(std::make_pair(ns->id(), ns));
    return ns;
}

class block *program::make_block(compiler::block* parent_scope, element_type_t block_type)
{
    auto block = new compiler::block(parent_scope, block_type);
    elements_.insert(std::make_pair(block->id(), block));
    return block;
}

boolean_literal *program::make_bool(compiler::block* parent_scope, bool value)
{
    auto bool_literal = new compiler::boolean_literal(parent_scope, value);
    elements_.insert(std::make_pair(bool_literal->id(), bool_literal));
    return bool_literal;
}

float_literal *program::make_float(compiler::block* parent_scope, double value)
{
    auto float_literal = new compiler::float_literal(parent_scope, value);
    elements_.insert(std::make_pair(float_literal->id(), float_literal));
    return float_literal;
}

integer_literal *program::make_integer(compiler::block* parent_scope, uint64_t value)
{
    auto integer_literal = new compiler::integer_literal(parent_scope, value);
    elements_.insert(std::make_pair(integer_literal->id(), integer_literal));
    return integer_literal;
}

string_literal *program::make_string(compiler::block* parent_scope, const std::string &value)
{
    auto string_literal = new compiler::string_literal(parent_scope, value);
    elements_.insert(std::make_pair(string_literal->id(), string_literal));
    return string_literal;
}

compiler::identifier *program::find_identifier(const ast_node_shared_ptr &node)
{
	auto var = (identifier*) nullptr;
	if (node->is_qualified_symbol()) {
		auto block_scope = block();
		for (const auto& symbol_node : node->children) {
			var = block_scope->identifiers().find(symbol_node->token.value);
			if (var == nullptr || var->initializer() == nullptr) {
                return nullptr;
            }

			auto expr = var->initializer()->expression();
			if (expr->element_type() == element_type_t::namespace_e) {
				auto ns = dynamic_cast<namespace_element*>(expr);
				block_scope = dynamic_cast<compiler::block*>(ns->expression());
			} else {
				break;
			}
		}
	} else {
		const auto& symbol_part = node->children[0];
		auto block_scope = current_scope();
		while (block_scope != nullptr) {
			var = block_scope->identifiers().find(symbol_part->token.value);
			if (var != nullptr) {
                return var;
            }
			block_scope = block_scope->parent_scope();
		}
		return nullptr;
	}
	return var;
}

const element_map_t &program::elements() const
{
	return elements_;
}

compiler::identifier *program::add_identifier_to_scope(result &r, const ast_node_shared_ptr &symbol,
	const ast_node_shared_ptr &rhs, compiler::block* parent_scope)
{
	auto namespace_type = find_type("namespace");
    auto type_find_result = find_identifier_type(r, symbol);

	auto scope = symbol->is_qualified_symbol() ? block()
		: (parent_scope != nullptr ? parent_scope : current_scope());

	for (size_t i = 0; i < symbol->children.size() - 1; i++) {
		const auto& symbol_node = symbol->children[i];
		auto var = scope->identifiers().find(symbol_node->token.value);
		if (var == nullptr) {
			auto new_scope = make_block(scope, element_type_t::block);
            auto ns = make_namespace(scope, new_scope, symbol_node->token.value);
			auto ns_identifier = make_identifier(scope, symbol_node->token.value, make_initializer(scope, ns));
			ns_identifier->type(namespace_type);
			ns_identifier->inferred_type(true);
            scope->blocks().push_back(new_scope);
			scope->identifiers().add(ns_identifier);
			scope = new_scope;
		} else {
			auto expr = var->initializer()->expression();
			if (expr->element_type() == element_type_t::namespace_e) {
				auto ns = dynamic_cast<namespace_element*>(expr);
				scope = dynamic_cast<compiler::block*>(ns->expression());
			} else {
                r.add_message("P018", "only a namespace is valid within a qualified name.", true);
                return nullptr;
			}
		}
	}

	const auto& final_symbol = symbol->children.back();

    compiler::element* init_expr = nullptr;
	compiler::initializer * init = nullptr;
	if (rhs != nullptr) {
        push_scope(scope);
        init_expr = evaluate(r, rhs);
        pop_scope();

        if (init_expr->element_type() == element_type_t::namespace_e) {
            auto ns = dynamic_cast<compiler::namespace_element*>(init_expr);
            ns->name(final_symbol->token.value);
        }
		if (init_expr != nullptr && init_expr->is_constant()) {
			init = make_initializer(scope, init_expr);
		}
	}

	auto new_identifier = make_identifier(scope, final_symbol->token.value, init);
	if (type_find_result.type == nullptr) {
        type_find_result.type = init_expr->infer_type(this);
		new_identifier->inferred_type(type_find_result.type != nullptr);
	}
	new_identifier->type(type_find_result.type);
	if (type_find_result.type == nullptr) {
		new_identifier->type(unknown_type_from_result(r, scope, new_identifier, type_find_result));
    }
	new_identifier->constant(symbol->is_constant_expression());
	scope->identifiers().add(new_identifier);
	if (init != nullptr
		&& init->expression()->element_type() == element_type_t::proc_type) {
		add_procedure_instance(r, dynamic_cast<procedure_type*>(init->expression()), rhs);
	}
    if (init == nullptr && init_expr != nullptr) {
        if (new_identifier->type()->element_type() == element_type_t::unknown_type) {
            r.add_message("P019", fmt::format("unable to infer type: {}", new_identifier->name()), true);
            return nullptr;
        } else {
            auto assign_bin_op = make_binary_operator(scope, operator_type_t::assignment, new_identifier, init_expr);
            add_expression_to_scope(scope, make_statement(current_scope(), label_list_t {}, assign_bin_op));
        }
    }

    return new_identifier;
}

array_type *program::make_array_type(result &r, compiler::block* parent_scope, compiler::type *entry_type, size_t size)
{
    std::string name = fmt::format("__array_{}_{}__", entry_type->name(), size);
    return make_type<array_type>(r, parent_scope, name, entry_type, size);
}

compiler::type *program::find_array_type(compiler::type *entry_type, size_t size) const
{
	return find_type(fmt::format("__array_{}_{}__", entry_type->name(), size));
}

namespace_type *program::make_namespace_type(result &r, compiler::block* parent_scope)
{
    return make_type<namespace_type>(r,  parent_scope);
}

void program::add_composite_type_fields(result &r, composite_type *type, const ast_node_shared_ptr &block)
{
	auto u32_type = find_type("u32");

	for (const auto& child : block->children) {
		if (child->type != ast_node_types_t::statement) {
			// TODO: this is an error!
			break;
		}
		auto expr_node = child->rhs;
		switch (expr_node->type) {
			case ast_node_types_t::assignment: {
				auto symbol_node = expr_node->lhs->children[0];
                auto type_find_result = find_identifier_type(r, symbol_node);
                auto init = make_initializer(current_scope(), evaluate(r, expr_node->rhs));
                auto field_identifier = make_identifier(current_scope(), symbol_node->children[0]->token.value, init);
                if (type_find_result.type ==nullptr) {
                    type_find_result.type = init->expression()->infer_type(this);
                    field_identifier->inferred_type(type_find_result.type != nullptr);
                }
                field_identifier->type(type_find_result.type);
                auto new_field = make_field(current_scope(), field_identifier);
                new_field->parent_element(type);
				type->fields().add(new_field);
				break;
			}
			case ast_node_types_t::symbol: {
				auto type_find_result = find_identifier_type(r, expr_node);
				auto field_identifier = make_identifier(current_scope(), expr_node->children[0]->token.value,
					nullptr);
                if (type_find_result.type == nullptr) {
                    if (type->type() == composite_types_t::enum_type) {
                        field_identifier->type(u32_type);
                    } else {
                        field_identifier->type(unknown_type_from_result(r, current_scope(), field_identifier,
                            type_find_result));
                    }
                } else {
                    field_identifier->type(type_find_result.type);
                }
                auto new_field = make_field(current_scope(), field_identifier);
                new_field->parent_element(type);
                type->fields().add(new_field);
				break;
			}
			default:
				break;
		}
	}
}

void program::add_procedure_instance(result &r, procedure_type *proc_type, const ast_node_shared_ptr &node)
{
	if (node->children.empty()) {
		return;
	}

	for (const auto& child_node : node->children) {
		switch (child_node->type) {
			case ast_node_types_t::attribute: {
                auto attribute = make_attribute(proc_type->scope(), child_node->token.value,
                                                evaluate(r, child_node->lhs));
                attribute->parent_element(proc_type);
                proc_type->attributes().add(attribute);
				break;
			}
			case ast_node_types_t::basic_block: {
                push_scope(proc_type->scope());
				auto basic_block = dynamic_cast<compiler::block*>(evaluate(r, child_node,
					 element_type_t::proc_instance_block));
                pop_scope();
                auto instance = make_procedure_instance(
                    proc_type->scope(), proc_type, basic_block);
                instance->parent_element(proc_type);
				proc_type->instances().push_back(instance);
			}
			default:
				break;
		}
	}
}

bool program::compile(result& r, assembly_listing& listing, const ast_node_shared_ptr& root)
{
	block_ = push_new_block();
	initialize_core_types(r);
	if (!compile_module(r, listing, root)) {
		return false;
	}

    // process directives
    visit_blocks(r, [&](compiler::block* scope) {
      for (auto stmt : scope->statements()) {
          if (stmt->expression()->element_type() == element_type_t::directive) {
              auto directive_element = dynamic_cast<compiler::directive*>(stmt->expression());
              if (!directive_element->execute(r, this)) {
                  return false;
              }
          }
      }
      return true;
    });

    // resolve unknown identifiers
    visit_blocks(r, [&](compiler::block* scope) {
        return true;
    });

	if (!resolve_unknown_types(r)) {
		return false;
	}

    emit_context_t context(terp_, &assembler_, this);
    visit_blocks(r, [&](compiler::block* scope) {
        scope->emit(r, context);
        return true;
    });

    auto root_block = assembler_.root_block();
    root_block->disassemble(listing);
	return !r.is_failed();
}

bool program::compile_module(result& r, assembly_listing& listing, const ast_node_shared_ptr& root)
{
	evaluate(r, root);
	return !r.is_failed();
}

bool program::run(result &r)
{
	while (!terp_->has_exited()) {
		if (!terp_->step(r)) {
			return false;
		}
	}
	return true;
}

type* program::find_type(const std::string& name) const
{
    auto scope = current_scope();
    while (scope != nullptr) {
        auto type = scope->types().find(name);
        if (type != nullptr) {
            return type;
        }
        auto type_identifier = scope->identifiers().find(name);
        if (type_identifier != nullptr) {
            return type_identifier->type();
        }
        scope = scope->parent_scope();
    }
    return nullptr;
}

type* program::find_type_down(const std::string &name)
{
    std::function<compiler::type* (compiler::block*)> recursive_find =
        [&](compiler::block* scope) -> compiler::type* {
          auto matching_type = scope->types().find(name);
          if (matching_type != nullptr) {
              return matching_type;
          }
          auto type_identifer = scope->identifiers().find(name);
          if (type_identifer != nullptr) {
              return type_identifer->type();
          }
          for (auto block : scope->blocks()) {
              return recursive_find(block);
          }
          return nullptr;
        };
    return recursive_find(block());
}

unknown_type *program::make_unknown_type(result &r, compiler::block *parent_scope, const std::string &name,
	bool is_array, size_t array_size)
{
    return make_type<unknown_type>(r, parent_scope, name, is_array, array_size);
}

void program::remove_element(id_t id)
{
	auto item = find_element(id);
	if (item == nullptr) {
		return;
	}
	elements_.erase(id);
	delete item;
}

bool program::resolve_unknown_types(result& r)
{
	auto it = identifiers_with_unknown_types_.begin();
	while (it != identifiers_with_unknown_types_.end()) {
		auto var = *it;
		if (var->type() != nullptr &&  var->type()->element_type() != element_type_t::unknown_type) {
			it = identifiers_with_unknown_types_.erase(it);
			continue;
		}

		compiler::type* identifier_type = nullptr;
		if (var->initializer() == nullptr) {
			auto unknown_type = dynamic_cast<compiler::unknown_type*>(var->type());
			identifier_type = find_type_down(unknown_type->name());
			if (unknown_type->is_array()) {
				auto array_type = find_array_type(identifier_type, unknown_type->array_size());
				if (array_type == nullptr) {
					array_type = make_array_type(r, var->parent_scope(),
						identifier_type, unknown_type->array_size());
				}
				identifier_type = array_type;
			}

			if (identifier_type != nullptr) {
				var->type(identifier_type);
				remove_element(unknown_type->id());
			}
		} else {
			identifier_type = var->initializer()->expression()->infer_type(this);
			var->type(identifier_type);
		}

		if (identifier_type != nullptr) {
			var->inferred_type(true);
			it = identifiers_with_unknown_types_.erase(it);
		} else {
			++it;
			r.add_message("P004", fmt::format("unable to resolve type for identifier: {}", var->name()), true);
		}
	}

	return identifiers_with_unknown_types_.empty();
}

terp *program::terp()
{
	return terp_;
}

void program::add_expression_to_scope(compiler::block *scope, compiler::element *expr)
{
	switch (expr->element_type()) {
		case element_type_t::comment: {
            auto comm = dynamic_cast<compiler::comment*>(expr);
            scope->comments().push_back(comm);
            break;
        }
		case element_type_t::attribute: {
            auto attr = dynamic_cast<attribute*>(expr);
            scope->attributes().add(attr);
            break;
        }
		case element_type_t::statement: {
            auto state = dynamic_cast<statement*>(expr);
			scope->statements().push_back(state);
			break;
		}
		default:
			break;
	}
}

void program::apply_attributes(result& r, compiler::element* element, const ast_node_shared_ptr& node)
{
	for (auto it = node->children.begin(); it != node->children.end(); ++it) {
		const auto& child_node = *it;
		if (child_node->type == ast_node_types_t::attribute) {
            auto attr = dynamic_cast<attribute*>(evaluate(r, child_node));
            attr->parent_element(element);
			element->attributes().add(attr);
		}
	}
}

import* program::make_import(compiler::block* parent_scope, element* expr)
{
	auto import_element = new compiler::import(parent_scope, expr);
	elements_.insert(std::make_pair(import_element->id(), import_element));
	return import_element;
}

void program::add_type_to_scope(compiler::type *value)
{
    current_scope()->types().add(value);
}

type_find_result_t program::find_identifier_type(result& r, const ast_node_shared_ptr &symbol)
{
    type_find_result_t result{};
    if (symbol->rhs != nullptr) {
        result.type_name = symbol->rhs->token.value;
        result.is_array= symbol->rhs->is_array();
        result.array_size = 0; // XXX: this needs to be fixed!
        if (!result.type_name.empty()) {
            result.type  = find_type(result.type_name);
            if (result.is_array) {
                auto array_type = find_array_type(result.type, result.array_size);
                if (array_type == nullptr) {
                    array_type = make_array_type(r, current_scope(), result.type, result.array_size);
                }
                result.type = array_type;
            }
        }
    }
    return result;
}

unknown_type *program::unknown_type_from_result(result &r,compiler::block *scope,
    compiler::identifier *identifier, type_find_result_t &result)
{
    auto type = make_unknown_type(r, scope, result.type_name, result.is_array, result.array_size);
    identifiers_with_unknown_types_.push_back(identifier);
    return type;
}

type_info *program::make_type_info_type(result &r, compiler::block *parent_scope)
{
    return make_type<type_info>(r, parent_scope);
}

bool program::within_procedure_scope(compiler::block* parent_scope) const
{
    auto block_scope = parent_scope == nullptr ? current_scope() : parent_scope;
    while (block_scope != nullptr) {
        if (block_scope->element_type() == element_type_t::proc_type_block
            ||  block_scope->element_type() == element_type_t::proc_instance_block) {
            return true;
        }

        block_scope = block_scope->parent_scope();
    }
    return false;
}

bool program::visit_blocks(result &r, const block_visitor_callable &callable,
    compiler::block* root_block)
{
    std::function<bool (compiler::block*)> recursive_execute =
        [&](compiler::block* scope) -> bool {
          if (!callable(scope)) {
              return false;
          }
          for (auto &block : scope->blocks()) {
              if (!recursive_execute(block)) {
                  return false;
              }
          }
          return true;
        };
    return recursive_execute(root_block != nullptr ? root_block : block());
}

tuple_type *program::make_tuple_type(result &r, compiler::block *parent_scope)
{
    return make_type<tuple_type>(r, parent_scope);
}
}