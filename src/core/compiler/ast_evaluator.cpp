//
// Created by 12132 on 2022/10/11.
//
#include <fmt/format.h>
#include "common/bytes.h"
#include "common/defer.h"
#include "ast_evaluator.h"
#include "elements/program.h"
#include "elements/module.h"
#include "elements/cast.h"
#include "elements/label.h"
#include "elements/alias.h"
#include "elements/import.h"
#include "elements/comment.h"
#include "elements/directive.h"
#include "elements/attribute.h"
#include "elements/if_element.h"
#include "elements/expression.h"
#include "elements/identifier.h"
#include "elements/statement.h"
#include "elements/element_types.h"
#include "elements/argument_list.h"
#include "elements/boolean_literal.h"
#include "elements/string_literal.h"
#include "elements/float_literal.h"
#include "elements/integer_literal.h"
#include "elements/return_element.h"
#include "elements/unary_operator.h"
#include "elements/symbol_element.h"
#include "elements/procedure_call.h"
#include "elements/binary_operator.h"
#include "elements/namespace_element.h"
#include "elements/procedure_instance.h"
#include "elements/identifier_reference.h"
#include "elements/module_reference.h"
#include "elements/types/type.h"
#include "elements/types/any_type.h"
#include "elements/types/bool_type.h"
#include "elements/types/tuple_type.h"
#include "elements/types/type_info.h"
#include "elements/types/string_type.h"
#include "elements/types/numeric_type.h"
#include "elements/types/unknown_type.h"
#include "elements/types/pointer_type.h"
#include "elements/types/module_type.h"
#include "elements/types/array_type.h"
#include "elements/types/procedure_type.h"
#include "elements/types/namespace_type.h"
#include "elements/types/composite_type.h"

namespace gfx::compiler {
ast_evaluator::ast_evaluator(element_builder *builder, compiler::program *program)
    : builder_(builder), program_(program)
{

}
element *ast_evaluator::evaluate_in_scope(result& r,compiler::session& session, const ast_node_shared_ptr& node,
                                    compiler::block* scope, element_type_t default_block_type)
{
    program_->push_scope(scope);
    auto result = evaluate(r, session, node, default_block_type);
    program_->pop_scope();
    return result;
}

element* ast_evaluator::evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node,
                           element_type_t default_block_type)
{
    if (node == nullptr) {
        return nullptr;
    }
    switch (node->type) {
        case ast_node_types_t::symbol: {
            return program_->make_symbol_from_node(r, node);
        }
        case ast_node_types_t::attribute: {
            auto attribute = builder_->make_attribute(program_->current_scope(), node->token.value, evaluate(r, session, node->lhs));
            attribute->location(node->location);
            return attribute;
        }
        case ast_node_types_t::directive: {
            auto expression = evaluate(r, session, node->lhs);
            auto directive_element = builder_->make_directive(program_->current_scope(), node->token.value, expression);
            directive_element->location(node->location);
            apply_attributes(r, session, directive_element, node);
            directive_element->evaluate(r, session, program_);
            return directive_element;
        }
        case ast_node_types_t::module: {
            auto module_block = builder_->make_block(program_->block_, element_type_t::module_block);
            auto module = builder_->make_module(program_->block_, module_block);
            module->source_file(session.current_source_file());
            program_->block_->blocks().push_back(module_block);

           program_->push_scope(module_block);
           program_->top_level_stack_.push(module_block);

            for (auto &it : node->children) {
                auto expr = evaluate(r, session, it, default_block_type);
                if (expr ==nullptr) {
                    return nullptr;
                }
                add_expression_to_scope(module_block, expr);
                expr->parent_element(module);
            }
           program_->top_level_stack_.pop();
            return module;
        }
        case ast_node_types_t::module_expression: {
            auto expr = resolve_symbol_or_evaluate(r, session, node->rhs);
            auto referene = builder_->make_module_reference(program_->current_scope(), expr);
            std::string path;
            if (expr->is_constant() && expr->as_string(path)) {
                std::filesystem::path source_path(path);
                auto cur_source_file = session.current_source_file();
                if (cur_source_file != nullptr && source_path.is_relative()) {
                    source_path = cur_source_file->path().parent_path() / source_path;
                }
                auto source_file = session.add_source_file(source_path);
                auto module = program_->compile_module(r, session, source_file);
                if (module == nullptr) {
                   program_->error(r, session,"C021", "unable laod moudle .", node->rhs->location);
                    return nullptr;
                }
                referene->module(module);
            } else {
                program_->error(r, session, "C021", "expected string literal or constant string variable.",
                      node->rhs->location);
                return nullptr;
            }
            return referene;
        }
        case ast_node_types_t::basic_block: {
            auto active_scope = program_->push_new_block(default_block_type);
            for (auto &it : node->children) {
                auto current_node = it;
                auto expr = evaluate(r, session, current_node, default_block_type);
                if (expr == nullptr) {
                    program_-> error(r, session, "C024", "invalid statement", current_node->location);
                    return nullptr;
                }
                add_expression_to_scope(active_scope, expr);
                expr->parent_element(active_scope);
            }
            return program_->pop_scope();
        }
        case ast_node_types_t::statement: {
            label_list_t labels{};
            if (node->lhs != nullptr) {
                for (const auto & label: node->lhs->children) {
                    labels.push_back(builder_->make_label(program_->current_scope(), label->token.value));
                }
            }
            auto expr = evaluate(r, session, node->rhs);
            if (expr == nullptr) {
                return nullptr;
            }

            if (expr->element_type() == element_type_t::symbol) {
                type_find_result_t find_type_result {};
                program_->find_identifier_type(r, find_type_result, node->rhs->rhs);
                expr = add_identifier_to_scope(r, session, dynamic_cast<compiler::symbol_element*>(expr),
                                               find_type_result, nullptr, 0);
            }
            return builder_->make_statement(program_->current_scope(), labels, expr);
        }
        case ast_node_types_t::expression: {
            auto expression = builder_->make_expression(program_->current_scope(), evaluate(r, session, node->lhs));
            expression->location(node->location);
            return expression;
        }
        case ast_node_types_t::assignment: {
            const auto &target_list = node->lhs;
            const auto& source_list = node->rhs;
            if (target_list->children.size() != source_list->children.size()) {
                program_->error(r,session,"P027","the number of left-hand-side targets must match"
                                       " the number of right-hand-side expressions.",
                      source_list->location);
                return nullptr;
            }
            identifier_list_t list {};
            for (size_t i = 0; i < target_list->children.size(); i++) {
                const auto& symbol_node = target_list->children[i];
                qualified_symbol_t qualified_symbol {};
                program_->make_qualified_symbol(qualified_symbol, symbol_node);
                auto existing_identifier = program_->find_identifier(qualified_symbol);
                if (existing_identifier != nullptr) {
                    auto rhs = evaluate(r, session, source_list->children[i]);
                    if (rhs == nullptr) {
                        return nullptr;
                    }
                    auto binary_op = builder_->make_binary_operator(program_->current_scope(), operator_type_t::assignment,
                                                                   existing_identifier, rhs);
                    apply_attributes(r, session, binary_op, node);
                    return binary_op;
                } else {
                    auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate(r, session, symbol_node));
                    type_find_result_t find_type_result {};
                    program_->find_identifier_type(r, find_type_result, symbol_node->rhs);
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
            return builder_->make_comment(program_->current_scope(), comment_type_t::line, node->token.value);
        }
        case ast_node_types_t::block_comment: {
            return builder_->make_comment(program_->current_scope(), comment_type_t::block, node->token.value);
        }
        case ast_node_types_t::string_literal: {
            auto string_literal =builder_->make_string(program_->current_scope(), node->token.value);
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
                            element = builder_->make_integer(program_->current_scope(), twos_complement(value));
                        } else {
                            element = builder_->make_integer(program_->current_scope(), value);
                        }
                        element->location(node->location);
                        return element;
                    } else {
                        program_->error(r, session, "P041", "invalid integer literal", node->location);
                        return nullptr;
                    }
                }
                case number_types_t::floating_point: {
                    // TODO: need to handle conversion failures
                    double value;
                    if (node->token.parse(value) == conversion_result_t::success) {
                        auto element = builder_->make_float(program_->current_scope(), value);
                        element->location(node->location);
                        return element;
                    }
                }
                default:
                    break;
            }
        }
        case ast_node_types_t::boolean_literal: {
            auto element = builder_->make_bool(program_->current_scope(), node->token.as_bool());
            element->location(node->location);
            return element;
        }
        case ast_node_types_t::import_expression: {
            qualified_symbol_t qualified_symbol {};
            program_-> make_qualified_symbol(qualified_symbol, node->lhs);

            compiler::identifier_reference* from_reference = nullptr;
            if (node->rhs != nullptr) {
                from_reference = dynamic_cast<compiler::identifier_reference*>(resolve_symbol_or_evaluate(
                    r, session, node->rhs));
                qualified_symbol.namespaces.insert(qualified_symbol.namespaces.begin(), from_reference->symbol().name);
            }
            auto identifier_reference =builder_->make_identifier_reference(program_->current_scope(),
                qualified_symbol, program_->find_identifier(qualified_symbol));
            auto import =builder_->make_import(program_->current_scope(), identifier_reference, from_reference,
                dynamic_cast<compiler::module*>(program_->current_top_level()->parent_element()));
            add_expression_to_scope(program_->current_scope(), import);
            return import;
        }
        case ast_node_types_t::if_expression:
        case ast_node_types_t::elseif_expression: {
            auto predicate = evaluate(r, session, node->lhs);
            auto true_branch = evaluate(r, session, node->children[0]);
            auto false_branch = evaluate(r, session, node->rhs);
            return builder_->make_if(program_->current_scope(), predicate, true_branch, false_branch);
        }
        case ast_node_types_t::else_expression: {
            return evaluate(r, session, node->children[0]);
        }
        case ast_node_types_t::unary_operator: {
            auto it = s_unary_operators.find(node->token.type);
            if (it == s_unary_operators.end()) {
                return nullptr;
            }
            return builder_->make_unary_operator(program_->current_scope(), it->second, resolve_symbol_or_evaluate(r, session, node->rhs));
        }
        case ast_node_types_t::binary_operator: {
            auto it = s_binary_operators.find(node->token.type);
            if (it == s_binary_operators.end()) {
                return nullptr;
            }
            auto lhs = resolve_symbol_or_evaluate(r, session, node->lhs);
            auto rhs = resolve_symbol_or_evaluate(r, session, node->rhs);
            return builder_->make_binary_operator(program_->current_scope(), it->second, lhs, rhs);
        }
        case ast_node_types_t::argument_list: {
            auto args = builder_->make_argument_list(program_->current_scope());
            for (const auto& arg_node : node->children) {
                auto arg = resolve_symbol_or_evaluate(r, session, arg_node);
                args->add(arg);
                arg->parent_element(args);
            }
            return args;
        }
        case ast_node_types_t::proc_call: {
            qualified_symbol_t qualified_symbol {};
            program_->make_qualified_symbol(qualified_symbol, node->lhs);
            auto proc_identifier = program_->find_identifier(qualified_symbol);
            argument_list* args = nullptr;
            auto expr = evaluate(r, session, node->rhs);
            if (expr != nullptr) {
                args = dynamic_cast<argument_list*>(expr);
            }
            auto element =builder_->make_procedure_call(program_->current_scope(),builder_->make_identifier_reference(
                program_->current_scope(), qualified_symbol, proc_identifier), args);
            element->location(node->location);
            return element;
        }
        case ast_node_types_t::proc_expression: {
            auto active_scope = program_->current_scope();
            auto block_scope = builder_->make_block(active_scope, element_type_t::proc_type_block);
            auto proc_type = builder_->make_procedure_type(r, active_scope, block_scope);
            program_->current_scope()->types().add(proc_type);

            auto count = 0;
            for (const auto& type_node : node->lhs->children) {
                switch (type_node->type) {
                    case ast_node_types_t::symbol: {
                        auto return_identifier = builder_->make_identifier(block_scope,
                            builder_->make_symbol(block_scope, fmt::format("_{}", count++)), nullptr);
                        return_identifier->usage(identifier_usage_t::stack);
                        auto type_name = type_node->children[0]->token.value;
                        return_identifier->type(program_->find_type(qualified_symbol_t{.name = type_name}));
                        auto new_field = builder_->make_field(block_scope, return_identifier);
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
                        program_->find_identifier_type(r, find_type_result, first_target->rhs, block_scope);
                        auto param_identifier = add_identifier_to_scope(r, session, symbol,
                                                                        find_type_result, param_node, 0, block_scope);
                        param_identifier->usage(identifier_usage_t::stack);
                        auto field = builder_->make_field(program_->current_scope(), param_identifier);
                        proc_type->parameters().add(field);
                        field->parent_element(proc_type);
                        break;
                    }
                    case ast_node_types_t ::symbol: {
                        auto symbol = dynamic_cast<symbol_element*>(evaluate_in_scope(r, session, param_node, block_scope));
                        type_find_result_t find_type_result {};
                        program_->find_identifier_type(r, find_type_result, param_node->rhs, block_scope);
                        auto param_identifier = add_identifier_to_scope(r, session, symbol, find_type_result, nullptr, 0, block_scope);
                        if (param_identifier !=nullptr) {
                            param_identifier->usage(identifier_usage_t::stack);
                            auto field = builder_->make_field(block_scope, param_identifier);
                            proc_type->parameters().add(field);
                            field->parent_element(proc_type);
                        } else {
                            program_->error(r, session, "P014", fmt::format("invalid parameter declaration: {}", symbol->name()),
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
            auto active_scope = program_->current_scope();
            auto enum_type = builder_->make_enum_type(r, active_scope,builder_->make_block(active_scope, element_type_t::block));
            program_->current_scope()->types().add(enum_type);
            add_composite_type_fields(r, session, enum_type, node->rhs);
            if (!enum_type->initialize(r, program_)) {
                return nullptr;
            }
            return enum_type;
        }
        case ast_node_types_t::cast_expression: {
            auto type_name = node->lhs->lhs->children[0]->token.value;
            auto type = program_->find_type(qualified_symbol_t{.name = type_name});
            if (type == nullptr) {
                program_->error(r, session, "P002", fmt::format("unknown type '{}'.", type_name), node->lhs->lhs->location);
                return nullptr;
            }
            auto cast = builder_->make_cast(program_->current_scope(), type, resolve_symbol_or_evaluate(r, session, node->rhs));
            cast->location(node->location);
            return cast;
        }
        case ast_node_types_t::alias_expression: {
            auto alias = builder_->make_alias(program_->current_scope(), evaluate(r, session, node->lhs));
            alias->location(node->location);
            return alias;
        }
        case ast_node_types_t::union_expression: {
            auto active_block = program_->current_scope();
            auto union_type = builder_->make_union_type(r, active_block,
                                                       builder_->make_block(active_block, element_type_t::block));
            program_->current_scope()->types().add(union_type);
            add_composite_type_fields(r, session, union_type, node->rhs);
            if (!union_type->initialize(r, program_)) {
                return nullptr;
            }
            return union_type;
        }
        case ast_node_types_t::struct_expression: {
            auto active_scope = program_->current_scope();
            auto struct_type = builder_->make_struct_type(r, active_scope,builder_->make_block(active_scope, element_type_t::block));
            program_-> current_scope()->types().add(struct_type);
            add_composite_type_fields(r, session, struct_type, node->rhs);
            if (!struct_type->initialize(r, program_)) {
                return nullptr;
            }
            return struct_type;
        }
        case ast_node_types_t::return_statement: {
            auto return_element = builder_->make_return(program_->current_scope());
            auto& expressions = return_element->expressions();
            for (const auto& arg_node : node->rhs->children) {
                auto arg = resolve_symbol_or_evaluate(r, session, arg_node);
                expressions.push_back(arg);
                arg->parent_element(return_element);
            }
            return return_element;
        }

        case ast_node_types_t::namespace_expression: {
            return builder_->make_namespace(program_->current_scope(), evaluate(r, session, node->rhs, default_block_type));
        }
        default: {
            break;
        }
    }

    return nullptr;
}

void ast_evaluator::apply_attributes(result& r, compiler::session& session, compiler::element* element, const ast_node_shared_ptr& node)
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

identifier* ast_evaluator::add_identifier_to_scope(result &r, compiler::session& session, symbol_element *symbol, type_find_result_t& type_find_result,
                                             const ast_node_shared_ptr &node, size_t source_index, compiler::block* parent_scope)
{
    auto namespace_type = program_->find_type(qualified_symbol_t{.name = "namespace"});
    auto scope = symbol->is_qualified() ? program_->current_top_level()
                                        : (parent_scope != nullptr ? parent_scope : program_->current_scope());

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
            auto new_scope = builder_->make_block(scope, element_type_t::block);
            auto ns = builder_->make_namespace(scope, new_scope);
            auto ns_identifier = builder_->make_identifier(scope, builder_->make_symbol(parent_scope, namespace_name),
                                                          builder_->make_initializer(scope, ns));
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
                program_->error(r, session, "P018", "only a namespace is valid within a qualified name.", node->lhs->location);
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
                init_expr = builder_->make_identifier_reference(scope,
                                                               init_symbol->qualified_symbol(), nullptr);
            }
            if (init_expr->is_constant()) {
                init = builder_->make_initializer(scope, init_expr);
            }
        }
    }

    auto new_identifier =  builder_->make_identifier(scope, symbol, init);
    apply_attributes(r, session, new_identifier, node);
    if (init_expr != nullptr) {
        if (init ==nullptr) {
            init_expr->parent_element(new_identifier);
        } else {
            auto folded_expr = init_expr->fold(r, program_);
            if (folded_expr != nullptr) {
                init_expr = folded_expr;
                auto old_expr = init->expression();
                init->expression(init_expr);
                program_->elements_.remove(old_expr->id());
            }
        }
    }
    if (type_find_result.type == nullptr) {
        if (init_expr != nullptr) {
            type_find_result.type = init_expr->infer_type(program_);
            new_identifier->type(type_find_result.type);
            new_identifier->inferred_type(type_find_result.type != nullptr);
        }
        if (type_find_result.type == nullptr) {
            new_identifier->type(program_->unknown_type_from_result(r, scope, new_identifier, type_find_result));
        }
    } else {
        new_identifier->type(type_find_result.type);
    }

    scope->identifiers().add(new_identifier);
    if (init != nullptr && init->expression()->element_type() == element_type_t::proc_type) {
        add_procedure_instance(r, session, dynamic_cast<procedure_type*>(init->expression()), source_node);
    }

    if (init == nullptr && init_expr == nullptr && new_identifier->type() == nullptr) {
        program_->error(r, session, "P019", fmt::format("unable to infer type: {}", new_identifier->symbol()->name()),
              new_identifier->symbol()->location());
        return nullptr;
    } else {
        if (init == nullptr && init_expr != nullptr) {
            auto assign_bin_op = builder_->make_binary_operator(scope, operator_type_t::assignment, new_identifier, init_expr);
            auto statement = builder_->make_statement(scope, label_list_t{}, assign_bin_op);
            add_expression_to_scope(scope, statement);
            statement->parent_element(scope);
        }
    }

    return new_identifier;
}

void ast_evaluator::add_composite_type_fields(result &r, compiler::session& session, composite_type *type, const ast_node_shared_ptr &block)
{
    auto u32_type = program_->find_type(qualified_symbol_t{.name= "u32"});

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
                    program_-> find_identifier_type(r, type_find_result, source_list->children[i], type->scope());
                    auto init = builder_->make_initializer(type->scope(), evaluate_in_scope(r, session,
                                                                                           source_list->children[i], type->scope()));
                    auto field_identifier = builder_->make_identifier(type->scope(), symbol, init);
                    if (type_find_result.type == nullptr) {
                        type_find_result.type = init->expression()->infer_type(program_);
                        field_identifier->inferred_type(type_find_result.type != nullptr);
                    }
                    field_identifier->type(type_find_result.type);
                    auto new_field = builder_->make_field(type->scope(), field_identifier);
                    new_field->parent_element(type);
                    type->fields().add(new_field);
                }
                break;
            }
            case ast_node_types_t::symbol: {
                auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(r, session, expr_node, type->scope()));
                type_find_result_t type_find_result {};
                program_-> find_identifier_type(r, type_find_result, expr_node->rhs, type->scope());
                auto field_identifier = builder_->make_identifier(type->scope(), symbol, nullptr);
                if (type_find_result.type == nullptr) {
                    if (type->type() == composite_types_t::enum_type) {
                        field_identifier->type(u32_type);
                    } else {
                        field_identifier->type(program_->unknown_type_from_result(r, type->scope(), field_identifier,
                                                                        type_find_result));
                    }
                } else {
                    field_identifier->type(type_find_result.type);
                }
                auto new_field = builder_->make_field(type->scope(), field_identifier);
                new_field->parent_element(type);
                type->fields().add(new_field);
                break;
            }
            default:
                break;
        }
    }
}

void ast_evaluator::add_procedure_instance(result &r, compiler::session& session,
                                     procedure_type *proc_type, const ast_node_shared_ptr &node)
{
    if (node->children.empty()) {
        return;
    }

    for (const auto& child_node : node->children) {
        switch (child_node->type) {
            case ast_node_types_t::attribute: {
                auto attribute = builder_->make_attribute(proc_type->scope(), child_node->token.value,
                                                         evaluate(r, session, child_node->lhs));
                attribute->parent_element(proc_type);
                proc_type->attributes().add(attribute);
                break;
            }
            case ast_node_types_t::basic_block: {
                program_->push_scope(proc_type->scope());
                auto basic_block = dynamic_cast<compiler::block*>(evaluate_in_scope(r, session, child_node,
                                                                                    proc_type->scope(), element_type_t::proc_instance_block));
                program_->pop_scope();
                auto instance = builder_->make_procedure_instance(
                    proc_type->scope(), proc_type, basic_block);
                instance->parent_element(proc_type);
                proc_type->instances().push_back(instance);
            }
            default:
                break;
        }
    }
}

void ast_evaluator::add_expression_to_scope(compiler::block *scope, compiler::element *expr)
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
compiler::element* ast_evaluator::resolve_symbol_or_evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node)
{
    compiler::element* element = nullptr;
    if (node != nullptr && node->type == ast_node_types_t::symbol) {
        qualified_symbol_t qualified_symbol {};
        program_->make_qualified_symbol(qualified_symbol, node);
        element = builder_->make_identifier_reference(program_->current_scope(), qualified_symbol,
                                                     program_->find_identifier(qualified_symbol));
    } else {
        element = evaluate(r, session, node);
    }
    return element;
}

compiler::block* ast_evaluator::add_namespaces_to_scope(result& r, compiler::session& session,
    const ast_node_shared_ptr& node, compiler::symbol_element* symbol,
    compiler::block* parent_scope) {
    auto namespace_type = program_->find_type(qualified_symbol_t {
        .name = "namespace"
    });

    auto namespaces = symbol->namespaces();
    auto scope = parent_scope;
    string_list_t temp_list {};
    std::string namespace_name {};
    for (size_t i = 0; i < namespaces.size(); i++) {
        if (!namespace_name.empty())
            temp_list.push_back(namespace_name);
        namespace_name = namespaces[i];
        auto var = scope->identifiers().find(namespace_name);
        if (var == nullptr) {
            auto new_scope = builder_->make_block(scope, element_type_t::block);
            auto ns = builder_->make_namespace(scope, new_scope);
            auto ns_identifier = builder_->make_identifier(scope,
                builder_->make_symbol(scope, namespace_name, temp_list),
                builder_->make_initializer(scope, ns));
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
                program_->error(r, session, "P018",
                    "only a namespace is valid within a qualified name.",
                    node->lhs->location);
                return nullptr;
            }
        }
    }
    return scope;
}


}