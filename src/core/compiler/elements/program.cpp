//
//
// Created by 12132 on 2022/4/23.
//

#include "program.h"
#include <fmt/format.h>
#include "module.h"
#include "cast.h"
#include "label.h"
#include "alias.h"
#include "import.h"
#include "comment.h"
#include "directive.h"
#include "attribute.h"
#include "if_element.h"
#include "expression.h"
#include "identifier.h"
#include "statement.h"
#include "common/bytes.h"
#include "element_types.h"
#include "common/defer.h"
#include "argument_list.h"
#include "boolean_literal.h"
#include "string_literal.h"
#include "float_literal.h"
#include "integer_literal.h"
#include "return_element.h"
#include "unary_operator.h"
#include "symbol_element.h"
#include "procedure_call.h"
#include "binary_operator.h"
#include "namespace_element.h"
#include "procedure_instance.h"
#include "identifier_reference.h"
#include "module_reference.h"
#include "types/type.h"
#include "types/any_type.h"
#include "types/bool_type.h"
#include "types/tuple_type.h"
#include "types/type_info.h"
#include "types/string_type.h"
#include "types/numeric_type.h"
#include "types/unknown_type.h"
#include "types/pointer_type.h"
#include "types/module_type.h"
#include "types/array_type.h"
#include "types/procedure_type.h"
#include "types/namespace_type.h"
#include "types/composite_type.h"

namespace gfx::compiler {

program::program(class terp* terp ,assembler *assembler)
	: element(nullptr, element_type_t::program), assembler_(assembler), terp_(terp)
    , builder_(this)
{
}

program::~program() = default;

bool program::compile(result& r, compiler::session& session)
{
    block_ = push_new_block();
    block_->parent_element(this);
    top_level_stack_.push(block_);
    initialize_core_types(r);
    for (const auto &source_file : session.source_files()) {
        auto module = compile_module(r, session, source_file);
        if (module == nullptr) {
            return false;
        }
    }
    // process directives
    auto directives = elements().find_by_type(element_type_t::directive);
    for (auto directive : directives) {
        auto directive_element = dynamic_cast<compiler::directive*>(directive);
        if (!directive_element->execute(r, session, this)) {
            return false;
        }
    }

    if (!resolve_unknown_identifiers(r)) {
        return false;
    }

    if (!resolve_unknown_types(r))  {
        return false;
    }

    if (!type_check(r, session)) {
        return false;
    }

    if (!r.is_failed()) {
        auto& listing = assembler_->listing();
        listing.add_source_file("top_level.basm");
        listing.select_source_file("top_level.basm");

        emit_context_t context(terp_, assembler_, this);
        emit(r, context);
        context.assembler->apply_addresses(r);
        context.assembler->resolve_labels(r);
        if (context.assembler->assemble(r)) {
            context.terp->run(r);
        }
    }
    top_level_stack_.pop();
    return !r.is_failed();
}

module *program::compile_module(result& r, compiler::session& session, source_file *source)
{
    auto is_root = session.current_source_file() == nullptr;
    session.push_source_file(source);
    defer({session.pop_source_file();});
    session.raise_phase(session_compile_phase_t::start, source->path());
    auto module_node = session.parse(r, source->path());
    compiler::module* module = nullptr;
    if (module_node != nullptr) {
        module = dynamic_cast<compiler::module*>(evaluate(r, session, module_node));
        if (module != nullptr) {
            module->parent_element(this);
            module->is_root(is_root);
        }
    }
    if (!r.is_failed()) {
        session.raise_phase(session_compile_phase_t::success, source->path());
    } else {
        session.raise_phase(session_compile_phase_t::failed, source->path());
    }
    return module;
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

void program::disassemble(FILE * file)
{
    auto root_block = assembler_->root_block();
    if (root_block == nullptr) {
        return;
    }
    root_block->disassemble();
    if (file != nullptr) {
        assembler_->listing().write(file);
    }
}

element *program::evaluate_in_scope(result& r,compiler::session& session, const ast_node_shared_ptr& node,
    compiler::block* scope, element_type_t default_block_type)
{
    push_scope(scope);
    auto result = evaluate(r, session, node, default_block_type);
    pop_scope();
    return result;
}

element* program::evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node,
   element_type_t default_block_type)
{
	if (node == nullptr) {
		return nullptr;
	}
	switch (node->type) {
		case ast_node_types_t::symbol: {
            return make_symbol_from_node(r, node);
		}
		case ast_node_types_t::attribute: {
			auto attribute = builder_.make_attribute(current_scope(), node->token.value, evaluate(r, session, node->lhs));
            attribute->location(node->location);
            return attribute;
		}
		case ast_node_types_t::directive: {
			auto expression = evaluate(r, session, node->lhs);
			auto directive_element = builder_.make_directive(current_scope(), node->token.value, expression);
            directive_element->location(node->location);
			apply_attributes(r, session, directive_element, node);
			directive_element->evaluate(r, session, this);
			return directive_element;
		}
		case ast_node_types_t::module: {
            auto module_block = builder_.make_block(block_, element_type_t::module_block);
            auto module = builder_.make_module(block_, module_block);
            module->source_file(session.current_source_file());
            block_->blocks().push_back(module_block);

            push_scope(module_block);
            top_level_stack_.push(module_block);

			for (auto &it : node->children) {
                auto expr = evaluate(r, session, it, default_block_type);
                if (expr ==nullptr) {
                    return nullptr;
                }
				add_expression_to_scope(module_block, expr);
                expr->parent_element(module);
			}
            top_level_stack_.pop();
			return module;
		}
        case ast_node_types_t::module_expression: {
            auto expr = resolve_symbol_or_evaluate(r, session, node->rhs);
            auto referene = builder_.make_module_reference(current_scope(), expr);
            std::string path;
            if (expr->is_constant() && expr->as_string(path)) {
                std::filesystem::path source_path(path);
                auto cur_source_file = session.current_source_file();
                if (cur_source_file != nullptr && source_path.is_relative()) {
                    source_path = cur_source_file->path().parent_path() / source_path;
                }
               auto source_file = session.add_source_file(source_path);
               auto module = compile_module(r, session, source_file);
               if (module == nullptr) {
                   error(r, session,"C021", "unable laod moudle .", node->rhs->location);
                   return nullptr;
               }
               referene->module(module);
            } else {
                error(r, session, "C021", "expected string literal or constant string variable.",
                    node->rhs->location);
                return nullptr;
            }
            return referene;
        }
		case ast_node_types_t::basic_block: {
			auto active_scope = push_new_block(default_block_type);
			for (auto &it : node->children) {
                auto current_node = it;
                auto expr = evaluate(r, session, current_node, default_block_type);
                if (expr == nullptr) {
                    error(r, session, "C024", "invalid statement", current_node->location);
                    return nullptr;
                }
                add_expression_to_scope(active_scope, expr);
                expr->parent_element(active_scope);
			}
			return pop_scope();
		}
		case ast_node_types_t::statement: {
			label_list_t labels{};
			if (node->lhs != nullptr) {
				for (const auto & label: node->lhs->children) {
                    labels.push_back(builder_.make_label(current_scope(), label->token.value));
				}
			}
            auto expr = evaluate(r, session, node->rhs);
            if (expr == nullptr) {
                return nullptr;
            }

            if (expr->element_type() == element_type_t::symbol) {
                type_find_result_t find_type_result {};
                find_identifier_type(r, find_type_result, node->rhs->rhs);
                expr = add_identifier_to_scope(r, session, dynamic_cast<compiler::symbol_element*>(expr),
                    find_type_result, nullptr, 0);
            }
			return builder_.make_statement(current_scope(), labels, expr);
		}
		case ast_node_types_t::expression: {
			auto expression = builder_.make_expression(current_scope(), evaluate(r, session, node->lhs));
            expression->location(node->location);
            return expression;
		}
		case ast_node_types_t::assignment: {
			const auto &target_list = node->lhs;
            const auto& source_list = node->rhs;
            if (target_list->children.size() != source_list->children.size()) {
                error(r,session,"P027","the number of left-hand-side targets must match"
                    " the number of right-hand-side expressions.",
                    source_list->location);
                return nullptr;
            }
			identifier_list_t list {};
            for (size_t i = 0; i < target_list->children.size(); i++) {
                const auto& symbol_node = target_list->children[i];
                qualified_symbol_t qualified_symbol {};
                make_qualified_symbol(qualified_symbol, symbol_node);
				auto existing_identifier = find_identifier(qualified_symbol);
				if (existing_identifier != nullptr) {
                    auto rhs = evaluate(r, session, source_list->children[i]);
                    if (rhs == nullptr) {
                        return nullptr;
                    }
                    auto binary_op = builder_.make_binary_operator(current_scope(), operator_type_t::assignment,
                      existing_identifier, rhs);
                    apply_attributes(r, session, binary_op, node);
					return binary_op;
				} else {
                    auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate(r, session, symbol_node));
                    type_find_result_t find_type_result {};
                    find_identifier_type(r, find_type_result, symbol_node->rhs);
					auto new_identifier = add_identifier_to_scope(r, session, symbol, find_type_result, node, i);
					if (new_identifier ==nullptr) {
                        return nullptr;
                    }
                    list.push_back(new_identifier);
				}
			}

            auto result = list.front();
			return result;
		}
		case ast_node_types_t::line_comment: {
			return builder_.make_comment(current_scope(), comment_type_t::line, node->token.value);
		}
		case ast_node_types_t::block_comment: {
			return builder_.make_comment(current_scope(), comment_type_t::block, node->token.value);
		}
		case ast_node_types_t::string_literal: {
			auto string_literal =builder_.make_string(current_scope(), node->token.value);
            string_literal->location(node->location);
            return string_literal;
		}
		case ast_node_types_t::number_literal: {
            // TODO: need to handle conversion failures
			switch (node->token.number_type) {
				case number_types_t::integer: {
					uint64_t value;
					if (node->token.parse(value) == conversion_result_t::success) {
                        compiler::element* element = nullptr;
                        if (node->token.is_signed()) {
                            element = builder_.make_integer(current_scope(), twos_complement(value));
                        } else {
                            element = builder_.make_integer(current_scope(), value);
                        }
                        element->location(node->location);
                        return element;
					} else {
                        error(r, session, "P041", "invalid integer literal", node->location);
                        return nullptr;
                    }
				}
				case number_types_t::floating_point: {
                    // TODO: need to handle conversion failures
					double value;
					if (node->token.parse(value) == conversion_result_t::success) {
                        auto element = builder_.make_float(current_scope(), value);
                        element->location(node->location);
                        return element;
					}
				}
				default:
					break;
			}
		}
		case ast_node_types_t::boolean_literal: {
            auto element = builder_.make_bool(current_scope(), node->token.as_bool());
            element->location(node->location);
            return element;
		}
		case ast_node_types_t::import_expression: {
            qualified_symbol_t qualified_symbol {};
            make_qualified_symbol(qualified_symbol, node->lhs);

            compiler::identifier_reference* from_reference = nullptr;
            if (node->rhs != nullptr) {
                from_reference = dynamic_cast<compiler::identifier_reference*>(resolve_symbol_or_evaluate(
                    r, session, node->rhs));
                qualified_symbol.namespaces.insert(qualified_symbol.namespaces.begin(), from_reference->symbol().name);
            }
            auto identifier_reference =builder_.make_identifier_reference(current_scope(),
                qualified_symbol, find_identifier(qualified_symbol));
            auto import =builder_.make_import(current_scope(), identifier_reference, from_reference,
                dynamic_cast<compiler::module*>(current_top_level()->parent_element()));
            add_expression_to_scope(current_scope(), import);
            return import;
		}
		case ast_node_types_t::if_expression:
		case ast_node_types_t::elseif_expression: {
			auto predicate = evaluate(r, session, node->lhs);
			auto true_branch = evaluate(r, session, node->children[0]);
			auto false_branch = evaluate(r, session, node->rhs);
			return builder_.make_if(current_scope(), predicate, true_branch, false_branch);
		}
        case ast_node_types_t::else_expression: {
            return evaluate(r, session, node->children[0]);
        }
		case ast_node_types_t::unary_operator: {
			auto it = s_unary_operators.find(node->token.type);
			if (it == s_unary_operators.end()) {
				return nullptr;
			}
			return builder_.make_unary_operator(current_scope(), it->second, resolve_symbol_or_evaluate(r, session, node->rhs));
		}
		case ast_node_types_t::binary_operator: {
			auto it = s_binary_operators.find(node->token.type);
			if (it == s_binary_operators.end()) {
				return nullptr;
			}
            auto lhs = resolve_symbol_or_evaluate(r, session, node->lhs);
            auto rhs = resolve_symbol_or_evaluate(r, session, node->rhs);
			return builder_.make_binary_operator(current_scope(), it->second, lhs, rhs);
		}
		case ast_node_types_t::argument_list: {
			auto args = builder_.make_argument_list(current_scope());
			for (const auto& arg_node : node->children) {
                auto arg = resolve_symbol_or_evaluate(r, session, arg_node);
				args->add(arg);
                arg->parent_element(args);
			}
			return args;
		}
		case ast_node_types_t::proc_call: {
            qualified_symbol_t qualified_symbol {};
            make_qualified_symbol(qualified_symbol, node->lhs);
			auto proc_identifier = find_identifier(qualified_symbol);
            argument_list* args = nullptr;
            auto expr = evaluate(r, session, node->rhs);
            if (expr != nullptr) {
                args = dynamic_cast<argument_list*>(expr);
            }
            auto element =builder_.make_procedure_call(current_scope(),builder_.make_identifier_reference(
                current_scope(), qualified_symbol, proc_identifier), args);
            element->location(node->location);
            return element;
		}
		case ast_node_types_t::proc_expression: {
			auto active_scope = current_scope();
			auto block_scope = builder_.make_block(active_scope, element_type_t::proc_type_block);
			auto proc_type = builder_.make_procedure_type(r, active_scope, block_scope);
			current_scope()->types().add(proc_type);

			auto count = 0;
			for (const auto& type_node : node->lhs->children) {
				switch (type_node->type) {
					case ast_node_types_t::symbol: {
						auto return_identifier = builder_.make_identifier(block_scope,
                           builder_.make_symbol(block_scope, fmt::format("_{}", count++)), nullptr);
                        return_identifier->usage(identifier_usage_t::stack);
                        auto type_name = type_node->children[0]->token.value;
                        return_identifier->type(find_type(qualified_symbol_t{.name = type_name}));
                        auto new_field = builder_.make_field(block_scope, return_identifier);
                        proc_type->returns().add(new_field);
                        new_field->parent_element(proc_type);
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
						const auto& first_target = param_node->lhs->children[0];
                        auto symbol = dynamic_cast<symbol_element*>(evaluate_in_scope(r, session, first_target, block_scope));
                        type_find_result_t find_type_result {};
                        find_identifier_type(r, find_type_result, first_target->rhs, block_scope);
						auto param_identifier = add_identifier_to_scope(r, session, symbol,
                            find_type_result, param_node, 0, block_scope);
                        param_identifier->usage(identifier_usage_t::stack);
                        auto field = builder_.make_field(current_scope(), param_identifier);
						proc_type->parameters().add(field);
                        field->parent_element(proc_type);
						break;
					}
					case ast_node_types_t ::symbol: {
                        auto symbol = dynamic_cast<symbol_element*>(evaluate_in_scope(r, session, param_node, block_scope));
                        type_find_result_t find_type_result {};
                        find_identifier_type(r, find_type_result, param_node->rhs, block_scope);
						auto param_identifier = add_identifier_to_scope(r, session, symbol, find_type_result, nullptr, 0, block_scope);
                        if (param_identifier !=nullptr) {
                            param_identifier->usage(identifier_usage_t::stack);
                            auto field = builder_.make_field(block_scope, param_identifier);
                            proc_type->parameters().add(field);
                            field->parent_element(proc_type);
                        } else {
                            error(r, session, "P014", fmt::format("invalid parameter declaration: {}", symbol->name()),
                                symbol->location());
                        }
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
			auto active_scope = current_scope();
			auto enum_type = builder_.make_enum_type(r, active_scope,builder_.make_block(active_scope, element_type_t::block));
            current_scope()->types().add(enum_type);
            add_composite_type_fields(r, session, enum_type, node->rhs);
            if (!enum_type->initialize(r, this)) {
                return nullptr;
            }
			return enum_type;
		}
		case ast_node_types_t::cast_expression: {
            auto type_name = node->lhs->lhs->children[0]->token.value;
			auto type = find_type(qualified_symbol_t{.name = type_name});
			if (type == nullptr) {
				error(r, session, "P002", fmt::format("unknown type '{}'.", type_name), node->lhs->lhs->location);
                return nullptr;
            }
			auto cast = builder_.make_cast(current_scope(), type, resolve_symbol_or_evaluate(r, session, node->rhs));
            cast->location(node->location);
            return cast;
		}
		case ast_node_types_t::alias_expression: {
			auto alias = builder_.make_alias(current_scope(), evaluate(r, session, node->lhs));
            alias->location(node->location);
            return alias;
		}
		case ast_node_types_t::union_expression: {
            auto active_block =  current_scope();
			auto union_type = builder_.make_union_type(r, active_block,
               builder_.make_block(active_block, element_type_t::block));
            current_scope()->types().add(union_type);
            add_composite_type_fields(r, session, union_type, node->rhs);
            if (!union_type->initialize(r, this)) {
                return nullptr;
            }
			return union_type;
		}
		case ast_node_types_t::struct_expression: {
            auto active_scope = current_scope();
			auto struct_type = builder_.make_struct_type(r, active_scope,builder_.make_block(active_scope, element_type_t::block));
            current_scope()->types().add(struct_type);
            add_composite_type_fields(r, session, struct_type, node->rhs);
            if (!struct_type->initialize(r, this)) {
                return nullptr;
            }
			return struct_type;
		}
		case ast_node_types_t::return_statement: {
			auto return_element = builder_.make_return(current_scope());
            auto& expressions = return_element->expressions();
			for (const auto& arg_node : node->rhs->children) {
                auto arg = resolve_symbol_or_evaluate(r, session, arg_node);
                expressions.push_back(arg);
                arg->parent_element(return_element);
			}
			return return_element;
		}

		case ast_node_types_t::namespace_expression: {
			return builder_.make_namespace(current_scope(), evaluate(r, session, node->rhs, default_block_type));
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
	auto scope_block = builder_.make_block(parent_scope, type);

	if (parent_scope != nullptr) {
        scope_block->parent_element(parent_scope);
		parent_scope->blocks().push_back(scope_block);
	}

	push_scope(scope_block);
	return scope_block;
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

    auto string_type = builder_.make_string_type(r, parent_scope,builder_.make_block(parent_scope, element_type_t::block));
	add_type_to_scope(string_type);
    add_type_to_scope(builder_.make_bool_type(r, parent_scope));
    auto module_type =builder_.make_module_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(module_type);

    auto namespace_type =builder_.make_namespace_type(r, parent_scope);
    add_type_to_scope(namespace_type);

    auto type_info_type =builder_.make_type_info_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(type_info_type);

    auto tuple_type = builder_.make_tuple_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
    add_type_to_scope(tuple_type);

    auto any_type = builder_.make_any_type(r, parent_scope, builder_.make_block(parent_scope, element_type_t::block));
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

compiler::identifier *program::find_identifier(const qualified_symbol_t& symbol, compiler::block* scope) const
{
    if (symbol.is_qualified()) {
        return dynamic_cast<compiler::identifier*>(walk_qualified_symbol(symbol, scope,
            [&symbol](compiler::block* scope) {
              return scope->identifiers().find(symbol.name);
            }));
	} else {
        return dynamic_cast<compiler::identifier*>(walk_parent_scopes(
            scope != nullptr ? scope : current_scope(),
            [&](compiler::block* scope) -> compiler::element* {
              auto var = scope->identifiers().find(symbol.name);
              if (var != nullptr) {
                  return var;
              }
              for (auto import : scope->imports()) {
                  auto identifier_reference = dynamic_cast<compiler::identifier_reference*>(import->expression());
                  auto qualified_symbol = identifier_reference->symbol();
                  qualified_symbol.namespaces.push_back(qualified_symbol.name);
                  qualified_symbol.name = symbol.name;
                  qualified_symbol.fully_qualified_name = make_fully_qualified_name(qualified_symbol);
                  var = find_identifier(qualified_symbol, import->module()->scope());
                  if (var != nullptr) {
                      return var;
                  }
              }
              return nullptr;
            }));
	}
}


compiler::element* program::resolve_symbol_or_evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node)
{
    compiler::element* element = nullptr;
    if (node != nullptr && node->type == ast_node_types_t::symbol) {
        qualified_symbol_t qualified_symbol {};
        make_qualified_symbol(qualified_symbol, node);
        element = builder_.make_identifier_reference(current_scope(), qualified_symbol, find_identifier(qualified_symbol));
    } else {
        element = evaluate(r, session, node);
    }
    return element;
}

 identifier* program::add_identifier_to_scope(result &r, compiler::session& session, symbol_element *symbol, type_find_result_t& type_find_result,
     const ast_node_shared_ptr &node, size_t source_index, compiler::block* parent_scope)
{
	auto namespace_type = find_type(qualified_symbol_t{.name = "namespace"});
	auto scope = symbol->is_qualified() ? current_top_level()
		: (parent_scope != nullptr ? parent_scope : current_scope());

    auto namespaces = symbol->namespaces();
    string_list_t temp_list {};
    std::string namespace_name {};
	for (auto & i : namespaces) {
        if (!namespace_name.empty()) {
            temp_list.push_back(namespace_name);
        }
        namespace_name = i;
        auto var = scope->identifiers().find(namespace_name);
		if (var == nullptr) {
			auto new_scope = builder_.make_block(scope, element_type_t::block);
            auto ns = builder_.make_namespace(scope, new_scope);
			auto ns_identifier = builder_.make_identifier(scope, builder_.make_symbol(parent_scope, namespace_name),
               builder_.make_initializer(scope, ns));
			ns_identifier->type(namespace_type);
			ns_identifier->inferred_type(true);
            ns_identifier->parent_element(scope->parent_element());
            scope->blocks().push_back(new_scope);
			scope->identifiers().add(ns_identifier);
			scope = new_scope;
		} else {
			auto expr = var->initializer()->expression();
			if (expr->element_type() == element_type_t::namespace_e) {
				auto ns = dynamic_cast<namespace_element*>(expr);
				scope = dynamic_cast<compiler::block*>(ns->expression());
			} else {
                error(r, session, "P018", "only a namespace is valid within a qualified name.", node->lhs->location);
                return nullptr;
			}
		}
	}

    ast_node_shared_ptr source_node = nullptr;
    if (node != nullptr) {
        source_node = node->rhs->children[source_index];
    }

    compiler::element* init_expr = nullptr;
	compiler::initializer * init = nullptr;
	if (node != nullptr) {
        init_expr = evaluate_in_scope(r, session, source_node, scope);
        if (init_expr != nullptr) {
            // XXX: need to revisit this!!!
            if (init_expr->element_type() == element_type_t::symbol) {
                auto init_symbol = dynamic_cast<compiler::symbol_element*>(init_expr);
                init_expr = builder_.make_identifier_reference(scope,
                    init_symbol->qualified_symbol(), nullptr);
            }
            if (init_expr->is_constant()) {
                init = builder_.make_initializer(scope, init_expr);
            }
        }
	}

	auto new_identifier =  builder_.make_identifier(scope, symbol, init);
    apply_attributes(r, session, new_identifier, node);
    if (init_expr != nullptr) {
        if (init ==nullptr) {
            init_expr->parent_element(new_identifier);
        } else {
            auto folded_expr = init_expr->fold(r, this);
            if (folded_expr != nullptr) {
                init_expr = folded_expr;
                auto old_expr = init->expression();
                init->expression(init_expr);
                elements_.remove(old_expr->id());
            }
        }
    }
    if (type_find_result.type == nullptr) {
        if (init_expr != nullptr) {
            type_find_result.type = init_expr->infer_type(this);
            new_identifier->type(type_find_result.type);
            new_identifier->inferred_type(type_find_result.type != nullptr);
        }
        if (type_find_result.type == nullptr) {
            new_identifier->type(unknown_type_from_result(r, scope, new_identifier, type_find_result));
        }
	} else {
        new_identifier->type(type_find_result.type);
    }

	scope->identifiers().add(new_identifier);
	if (init != nullptr && init->expression()->element_type() == element_type_t::proc_type) {
		add_procedure_instance(r, session, dynamic_cast<procedure_type*>(init->expression()), source_node);
	}

    if (init == nullptr && init_expr == nullptr && new_identifier->type() == nullptr) {
        error(r, session, "P019", fmt::format("unable to infer type: {}", new_identifier->symbol()->name()),
            new_identifier->symbol()->location());
        return nullptr;
    } else {
        if (init == nullptr && init_expr != nullptr) {
            auto assign_bin_op = builder_.make_binary_operator(scope, operator_type_t::assignment, new_identifier, init_expr);
            auto statement = builder_.make_statement(scope, label_list_t{}, assign_bin_op);
            add_expression_to_scope(scope, statement);
            statement->parent_element(scope);
        }
    }

    return new_identifier;
}

void program::add_composite_type_fields(result &r, compiler::session& session, composite_type *type, const ast_node_shared_ptr &block)
{
	auto u32_type = find_type(qualified_symbol_t{.name= "u32"});

	for (const auto& child : block->children) {
		if (child->type != ast_node_types_t::statement) {
			// TODO: this is an error!
			break;
		}
		auto expr_node = child->rhs;
		switch (expr_node->type) {
			case ast_node_types_t::assignment: {
                const auto &target_list = expr_node->lhs;
                const auto &source_list = expr_node->rhs;
                for (size_t i = 0; i < target_list->children.size(); ++i) {
                    auto &symbol_node = target_list->children[i];
                    auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(r, session,
                        symbol_node,type->scope()));
                    type_find_result_t type_find_result {};
                    find_identifier_type(r, type_find_result, source_list->children[i], type->scope());
                    auto init = builder_.make_initializer(type->scope(), evaluate_in_scope(r, session,
                         source_list->children[i], type->scope()));
                    auto field_identifier = builder_.make_identifier(type->scope(), symbol, init);
                    if (type_find_result.type == nullptr) {
                        type_find_result.type = init->expression()->infer_type(this);
                        field_identifier->inferred_type(type_find_result.type != nullptr);
                    }
                    field_identifier->type(type_find_result.type);
                    auto new_field = builder_.make_field(type->scope(), field_identifier);
                    new_field->parent_element(type);
                    type->fields().add(new_field);
                }
				break;
			}
			case ast_node_types_t::symbol: {
                auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(r, session, expr_node, type->scope()));
                type_find_result_t type_find_result {};
                find_identifier_type(r, type_find_result, expr_node->rhs, type->scope());
				auto field_identifier = builder_.make_identifier(type->scope(), symbol, nullptr);
                if (type_find_result.type == nullptr) {
                    if (type->type() == composite_types_t::enum_type) {
                        field_identifier->type(u32_type);
                    } else {
                        field_identifier->type(unknown_type_from_result(r, type->scope(), field_identifier,
                            type_find_result));
                    }
                } else {
                    field_identifier->type(type_find_result.type);
                }
                auto new_field = builder_.make_field(type->scope(), field_identifier);
                new_field->parent_element(type);
                type->fields().add(new_field);
				break;
			}
			default:
				break;
		}
	}
}

void program::add_procedure_instance(result &r, compiler::session& session,
    procedure_type *proc_type, const ast_node_shared_ptr &node)
{
	if (node->children.empty()) {
		return;
	}

	for (const auto& child_node : node->children) {
		switch (child_node->type) {
			case ast_node_types_t::attribute: {
                auto attribute = builder_.make_attribute(proc_type->scope(), child_node->token.value,
                    evaluate(r, session, child_node->lhs));
                attribute->parent_element(proc_type);
                proc_type->attributes().add(attribute);
				break;
			}
			case ast_node_types_t::basic_block: {
                push_scope(proc_type->scope());
				auto basic_block = dynamic_cast<compiler::block*>(evaluate_in_scope(r, session, child_node,
                    proc_type->scope(), element_type_t::proc_instance_block));
                pop_scope();
                auto instance = builder_.make_procedure_instance(
                    proc_type->scope(), proc_type, basic_block);
                instance->parent_element(proc_type);
				proc_type->instances().push_back(instance);
			}
			default:
				break;
		}
	}
}

bool program::resolve_unknown_types(result& r)
{
	auto it = identifiers_with_unknown_types_.begin();
	while (it != identifiers_with_unknown_types_.end()) {
        auto var = *it;
        if (var->type()!=nullptr && var->type()->element_type()!=element_type_t::unknown_type) {
            it = identifiers_with_unknown_types_.erase(it);
            continue;
        }

        compiler::type *identifier_type = nullptr;
        if (var->is_parent_element(element_type_t::binary_operator)) {
            auto binary_operator = dynamic_cast<compiler::binary_operator *>(var->parent_element());
            if (binary_operator->operator_type()==operator_type_t::assignment) {
                identifier_type = binary_operator->rhs()->infer_type(this);
                var->type(identifier_type);
            }
        } else {
            if (var->initializer()==nullptr) {
                auto unknown_type = dynamic_cast<compiler::unknown_type *>(var->type());
                type_find_result_t find_result {};
                find_result.type_name = unknown_type->symbol()->qualified_symbol();
                find_result.is_array = unknown_type->is_array();
                find_result.is_pointer = unknown_type->is_pointer();
                find_result.array_size = unknown_type->array_size();
                identifier_type = make_complete_type(r, find_result, var->parent_scope());
                auto type_name = unknown_type->symbol()->name();
                identifier_type = find_type(qualified_symbol_t{.name = type_name});
                if (identifier_type != nullptr) {
                    var->type(identifier_type);
                    elements_.remove(unknown_type->id());
                }
            } else {
                identifier_type = var->initializer()->expression()->infer_type(this);
                var->type(identifier_type);
            }
        }
        if (identifier_type!=nullptr) {
            var->inferred_type(true);
            it = identifiers_with_unknown_types_.erase(it);
        } else {
            ++it;
            error(r, var, "P004", fmt::format("unable to resolve type for identifier: {}", var->symbol()->name()),
                  var->symbol()->location());
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
            scope->comments().emplace_back(comm);
            break;
        }
        case element_type_t::import_e: {
            auto import = dynamic_cast<compiler::import*>(expr);
            scope->imports().emplace_back(import);
            break;
        }
		case element_type_t::attribute: {
            auto attr = dynamic_cast<attribute*>(expr);
            scope->attributes().add(attr);
            break;
        }
		case element_type_t::statement: {
            auto state = dynamic_cast<statement*>(expr);
			scope->statements().emplace_back(state);
			break;
		}
		default:
			break;
	}
}

void program::apply_attributes(result& r, compiler::session& session, compiler::element* element, const ast_node_shared_ptr& node)
{
    if (node == nullptr) {
        return;
    }
	for (auto it = node->children.begin(); it != node->children.end(); ++it) {
		const auto& child_node = *it;
		if (child_node->type == ast_node_types_t::attribute) {
            auto attr = dynamic_cast<attribute*>(evaluate(r, session, child_node));
            attr->parent_element(element);
            auto &attributes = element->attributes();
			attributes.add(attr);
		}
	}
}

void program::add_type_to_scope(compiler::type *value)
{
    current_scope()->types().add(value);
}

bool program::find_identifier_type(result& r, type_find_result_t &result, const ast_node_shared_ptr &type_node,
    compiler::block* parent_scope)
{
    if (type_node == nullptr) {
        return false;
    }
    make_qualified_symbol(result.type_name, type_node->lhs);
    result.array_size = 0;
    result.is_array = type_node->is_array();
    result.is_spread = type_node->is_spread();
    result.is_pointer = type_node->is_pointer();
    make_complete_type(r, result, parent_scope);
    return result.type !=nullptr;
}

unknown_type *program::unknown_type_from_result(result &r,compiler::block *scope,
    compiler::identifier *identifier, type_find_result_t &result)
{
    auto symbol = builder_.make_symbol(scope, result.type_name.name, result.type_name.namespaces);
    auto type = builder_.make_unknown_type(r, scope, symbol, result.is_pointer, result.is_array, result.array_size);
    identifiers_with_unknown_types_.push_back(identifier);
    return type;
}

void program::make_qualified_symbol(qualified_symbol_t& symbol, const ast_node_shared_ptr& node)
{
    if (!node->children.empty()) {
        for (size_t i = 0; i < node->children.size() - 1; i++) {
            symbol.namespaces.push_back(node->children[i]->token.value);
        }
    }
    symbol.name = node->children.back()->token.value;
    symbol.location = node->location;
    symbol.fully_qualified_name = make_fully_qualified_name(symbol);
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
    return recursive_execute(root_block != nullptr ? root_block : top_level_stack_.top());
}


bool program::on_emit(result &r, emit_context_t &context)
{
    auto instruction_block = context.assembler->make_basic_block();
    instruction_block->jump_direct("_initializer");

    std::map<section_t, element_list_t> vars_by_section {};
    vars_by_section.insert(std::make_pair(section_t::bss,     element_list_t()));
    auto ro   = vars_by_section.insert(std::make_pair(section_t::ro_data, element_list_t()));
    auto data = vars_by_section.insert(std::make_pair(section_t::data,    element_list_t()));
    vars_by_section.insert(std::make_pair(section_t::text,    element_list_t()));

    auto identifiers = elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        auto var_type = var->type();
        if (var_type == nullptr || var_type->element_type() == element_type_t::namespace_type ) {
            continue;
        }

        if (within_procedure_scope(var->parent_scope()) || var->is_parent_element(element_type_t::field)) {
            continue;
        }

        switch (var->type()->element_type()) {
            case element_type_t::bool_type:
            case element_type_t::numeric_type: {
                if (var->is_constant()) {
                    auto& list = ro.first->second;
                    list.emplace_back(var);
                } else {
                    auto& list = data.first->second;
                    list.emplace_back(var);
                }
                break;
            }
            case element_type_t::any_type:
            case element_type_t::array_type:
            case element_type_t::tuple_type:
            case element_type_t::string_type:
            case element_type_t::composite_type: {
                if (var->initializer() != nullptr) {
                    if (var->is_constant()) {
                        auto& list = ro.first->second;
                        list.emplace_back(var);
                    } else {
                        auto& list = data.first->second;
                        list.emplace_back(var);
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    auto& ro_list = ro.first->second;

    for (const auto& it : interned_string_literals_) {
        compiler::string_literal* str = it.second.front();
        if (!str->is_parent_element(element_type_t::argument_list)) {
            continue;
        }
        ro_list.emplace_back(str);
    }
    std::vector<variable_t*> literals {};
    for (const auto& section : vars_by_section) {
        instruction_block->section(section.first);
        instruction_block->current_entry()->blank_lines(1);
        for (auto e : section.second) {
            switch (e->element_type()) {
                case element_type_t::string_literal: {
                    auto string_literal = dynamic_cast<compiler::string_literal*>(e);
                    instruction_block->memo();
                    instruction_block->align(4);
                    auto it = interned_string_literals_.find(string_literal->value());
                    if (it != interned_string_literals_.end()) {
                        auto current_entry = instruction_block->current_entry();
                        string_literal_list_t& str_list = it->second;
                        for (auto str : str_list) {
                            auto var_label = instruction_block->make_label(str->label_name());
                            current_entry->label(var_label);
                            auto var = context.allocate_variable(r, var_label->name(),
                                context.program->find_type({.name = "string"}), identifier_usage_t::heap);
                            if (var != nullptr) {
                                var->address_offset = 4;
                                literals.emplace_back(var);
                            }
                        }
                        current_entry->blank_lines(1);
                    }
                    instruction_block->current_entry()->comment(fmt::format("\"{}\"",
                        string_literal->value()));
                    instruction_block->string(string_literal->escaped_value());
                    break;
                }
                case element_type_t::identifier: {
                    auto var = dynamic_cast<compiler::identifier*>(e);
                    auto init = var->initializer();
                    instruction_block->memo();
                    instruction_block->current_entry()->blank_lines(1);
                    auto type_alignment = static_cast<uint8_t>(var->type()->alignment());
                    if (type_alignment > 1) {
                        instruction_block->align(type_alignment);
                    }

                    auto var_label = instruction_block->make_label(var->symbol()->name());
                    instruction_block->current_entry()->label(var_label);
                    context.allocate_variable(r, var_label->name(), var->type(),
                        identifier_usage_t::heap, nullptr);
                    switch (var->type()->element_type()) {
                        case element_type_t::bool_type: {
                            bool value = false;
                            var->as_bool(value);
                            if (init == nullptr) {
                                instruction_block->reserve_byte(1);
                            } else {
                                instruction_block->bytes({static_cast<uint8_t>(value ? 1 : 0)});
                            }
                            break;
                        }
                        case element_type_t::numeric_type: {
                            uint64_t value = 0;
                            if (var->type()->number_class() == type_number_class_t::integer) {
                                var->as_integer(value);
                            } else {
                                double temp = 0;
                                if (var->as_float(temp)) {
                                    register_value_alias_t alias {};
                                    alias.d = temp;
                                    value = alias.u;
                                }
                            }
                            auto symbol_type = integer_symbol_type_for_size(var->type()->size_in_bytes());
                            switch (symbol_type) {
                                case symbol_type_t::u8:
                                    if (init == nullptr) {
                                        instruction_block->reserve_byte(1);
                                    } else {
                                        instruction_block->bytes({static_cast<uint8_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::u16:
                                    if (init == nullptr) {
                                        instruction_block->reserve_word(1);
                                    } else {
                                        instruction_block->words({static_cast<uint16_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::f32:
                                case symbol_type_t::u32:
                                    if (init == nullptr) {
                                        instruction_block->reserve_dword(1);
                                    } else {
                                        instruction_block->dwords({static_cast<uint32_t>(value)});
                                    }
                                    break;
                                case symbol_type_t::f64:
                                case symbol_type_t::u64:
                                    if (init == nullptr) {
                                        instruction_block->reserve_qword(1);
                                    } else {
                                        instruction_block->qwords({value});
                                    }
                                    break;
                                case symbol_type_t::bytes:
                                default:
                                    break;
                            }
                            break;
                        }
                        case element_type_t::string_type: {
                            if (init != nullptr) {
                                auto string_literal = dynamic_cast<compiler::string_literal*>(init->expression());
                                instruction_block->current_entry()->comment(fmt::format("\"{}\"",
                                    string_literal->value()));
                                instruction_block->string(string_literal->value());
                            }
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    context.assembler->push_block(instruction_block);

    auto procedure_types = elements().find_by_type(element_type_t::proc_type);
    procedure_type_list_t proc_list {};
    for (auto p : procedure_types) {
        auto procedure_type = dynamic_cast<compiler::procedure_type*>(p);
        if (procedure_type->parent_scope()->element_type() == element_type_t::proc_instance_block) {
            proc_list.emplace_back(procedure_type);
        }
    }

    for (auto p : procedure_types) {
        auto procedure_type = dynamic_cast<compiler::procedure_type*>(p);
        if (procedure_type->parent_scope()->element_type() != element_type_t::proc_instance_block) {
            proc_list.emplace_back(procedure_type);
        }
    }

    for (auto procedure_type : proc_list) {
        procedure_type->emit(r, context);
    }

    auto top_level_block = context.assembler->make_basic_block();
    top_level_block->align(instruction_t::alignment);
    top_level_block->current_entry()->blank_lines(1);
    top_level_block->memo();
    top_level_block->current_entry()->label(top_level_block->make_label("_initializer"));
//    for (auto var : literals) {
//        var->init(context.assembler, top_level_block);
//    }

    block_list_t implicit_blocks {};
    auto module_blocks = elements().find_by_type(element_type_t::module_block);
    for (auto block : module_blocks) {
        implicit_blocks.emplace_back(dynamic_cast<compiler::block*>(block));
    }

    context.assembler->push_block(top_level_block);
    for (auto block : implicit_blocks) {
        block->emit(r, context);
    }

    auto finalizer_block = context.assembler->make_basic_block();
    finalizer_block->align(instruction_t::alignment);
    finalizer_block->current_entry()->blank_lines(1);
    finalizer_block->exit();
    finalizer_block->current_entry()->label(finalizer_block->make_label("_finalizer"));

    context.assembler->pop_block();
    context.assembler->pop_block();

    return true;
}

compiler::symbol_element* program::make_symbol_from_node(result& r, const ast_node_shared_ptr& node)
{
    qualified_symbol_t qualified_symbol {};
    make_qualified_symbol(qualified_symbol, node);
    auto symbol =builder_.make_symbol(current_scope(), qualified_symbol.name, qualified_symbol.namespaces);
    symbol->location(node->location);
    symbol->constant(node->is_constant_expression());
    return symbol;
}

compiler::type *program::find_type(const qualified_symbol_t &symbol, compiler::block* scope) const
{
    if (symbol.is_qualified()) {
        return dynamic_cast<compiler::type*>(walk_qualified_symbol(symbol,
            const_cast<compiler::program*>(this)->current_top_level(),
            [&](compiler::block* scope) -> compiler::element* {
              auto matching_type = scope->types().find(symbol.name);
              if (matching_type != nullptr) {
                  return matching_type;
              }
              auto type_identifier = find_identifier(symbol, scope);
              if (type_identifier != nullptr) {
                  return type_identifier->type();
              }
              return nullptr;
            }));
    } else {
        return dynamic_cast<compiler::type*>(walk_parent_scopes(scope != nullptr ? scope : current_scope(),
            [&](compiler::block* scope) -> compiler::element* {
              auto type = scope->types().find(symbol.name);
              if (type != nullptr) {
                  return type;
              }
              auto type_identifier = find_identifier(symbol, scope);
              if (type_identifier != nullptr) {
                  return type_identifier->type();
              }
              return nullptr;
            }));
    }
}

bool program::resolve_unknown_identifiers(result &r)
{
    auto it = unresolved_identifier_references_.begin();
    while (it != unresolved_identifier_references_.end()) {
        auto unresolved_reference = *it;
        if (unresolved_reference->resolved()) {
            it = unresolved_identifier_references_.erase(it);
            continue;
        }

        auto identifier = find_identifier(unresolved_reference->symbol(),
            unresolved_reference->parent_scope());
        if (identifier == nullptr) {
            ++it;
            error(r, unresolved_reference, "P004",  fmt::format("unable to resolve identifier: {}", unresolved_reference->symbol().name),
                  unresolved_reference->symbol().location);
            continue;
        }
        unresolved_reference->identifier(identifier);
        it = unresolved_identifier_references_.erase(it);
    }

    return unresolved_identifier_references_.empty();
}

compiler::block *program::block() const
{
    return block_;
}

compiler::block *program::current_top_level()
{
    if (top_level_stack_.empty()) {
        return nullptr;
    }
    return top_level_stack_.top();
}

compiler::module *program::find_module(compiler::element *element)
{
    return dynamic_cast<compiler::module*>(walk_parent_elements(element,
        [](compiler::element* each) -> compiler::element* {
          if (each->element_type() == element_type_t::module)
              return each;
          return nullptr;
        }));
}

void program::error(result &r, compiler::session &session, const std::string &code,
                    const std::string &message, const source_location &location)
{
    auto source_file = session.current_source_file();
    if (source_file != nullptr) {
        source_file->error(r, code, message, location);
    }
}

void program::error(result &r, compiler::element *element, const std::string &code,
                    const std::string &message, const source_location &location)
{
    auto module = find_module(element);
    if (module != nullptr) {
        module->source_file()->error(r, code, message, location);
    }
}

element *program::walk_parent_scopes(compiler::block *scope, const program::scope_visitor_callable &callable)
{
    while (scope != nullptr) {
        auto* result = callable(scope);
        if (result != nullptr) {
            return result;
        }
        scope = scope->parent_scope();
    }
    return nullptr;
}

element *program::walk_parent_elements(compiler::element *element,
    const program::element_visitor_callable &callable)
{
    auto current = element;
    while (current != nullptr) {
        auto* result = callable(current);
        if (result != nullptr) {
            return result;
        }
        current = current->parent_element();
    }
    return nullptr;
}

element *program::walk_qualified_symbol(const qualified_symbol_t &symbol, compiler::block *scope,
                                        const program::namespace_visitor_callable &callable) const
{
    auto non_const_this = const_cast<compiler::program*>(this);
    auto block_scope = scope != nullptr ? scope : non_const_this->current_top_level();
    for (const auto& namespace_name : symbol.namespaces) {
        auto var = block_scope->identifiers().find(namespace_name);
        if (var == nullptr || var->initializer() == nullptr)
            return nullptr;
        auto expr = var->initializer()->expression();
        if (expr->element_type() == element_type_t::namespace_e) {
            auto ns = dynamic_cast<namespace_element*>(expr);
            block_scope = dynamic_cast<compiler::block*>(ns->expression());
        } else if (expr->element_type() == element_type_t::module_reference) {
            auto module_reference = dynamic_cast<compiler::module_reference*>(expr);
            block_scope = module_reference->module()->scope();
        } else {
            return nullptr;
        }
    }
    return callable(block_scope);
}

compiler::type *program::find_pointer_type(compiler::type *base_type, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::pointer_type::name_for_pointer(base_type)
    }, scope);
}

compiler::type *program::find_array_type(compiler::type *entry_type, size_t size, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::array_type::name_for_array(entry_type, size)
    }, scope);
}

compiler::type *program::make_complete_type(result &r, type_find_result_t &result, compiler::block *parent_scope)
{
    result.type = find_type(result.type_name, parent_scope);
    if (result.type != nullptr) {
        if (result.is_array) {
            auto array_type = find_array_type(result.type, result.array_size, parent_scope);
            if (array_type == nullptr) {
                array_type =builder_.make_array_type(r,parent_scope, builder_.make_block(parent_scope, element_type_t::block),
                    result.type, result.array_size);
            }
            result.type = array_type;
        }

        if (result.is_pointer) {
            auto pointer_type = find_pointer_type(result.type, parent_scope);
            if (pointer_type == nullptr) {
                pointer_type = builder_.make_pointer_type(r, parent_scope, result.type);
            }
            result.type = pointer_type;
        }
    }
    return result.type;
}

element_builder &program::builder()
{
    return builder_;
}

element_map &program::elements()
{
    return elements_;
}

bool program::type_check(result &r, session &session)
{
    auto identifiers = elements().find_by_type(element_type_t::identifier);
    for (auto identifier : identifiers) {
        auto var = dynamic_cast<compiler::identifier*>(identifier);
        auto init = var->initializer();
        if (init == nullptr)
            continue;
        auto rhs_type = init->infer_type(this);
        if (!var->type()->type_check(rhs_type)) {
            error(
                r,
                init,
                "C051",
                fmt::format(
                    "type mismatch: cannot assign {} to {}.",
                    rhs_type->symbol()->name(),
                    var->type()->symbol()->name()),
                var->location());
        }
    }

    auto binary_ops = elements().find_by_type(element_type_t::binary_operator);
    for (auto op : binary_ops) {
        auto binary_op = dynamic_cast<compiler::binary_operator*>(op);
        if (binary_op->operator_type() != operator_type_t::assignment)
            continue;

        // XXX: revisit this for destructuring/multiple assignment
        auto var = dynamic_cast<compiler::identifier*>(binary_op->lhs());
        auto rhs_type = binary_op->rhs()->infer_type(this);
        if (!var->type()->type_check(rhs_type)) {
            error(r, binary_op, "C051",
                fmt::format( "type mismatch: cannot assign {} to {}.",
                    rhs_type->symbol()->name(), var->type()->symbol()->name()),
                binary_op->rhs()->location());
        }
    }

    return !r.is_failed();
}

}