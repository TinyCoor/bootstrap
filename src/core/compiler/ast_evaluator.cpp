//
// Created by 12132 on 2022/10/11.
//
#include <fmt/format.h>
#include "common/bytes.h"
#include "common/defer.h"
#include "ast_evaluator.h"
#include "elements/program.h"
#include "elements/module.h"
#include "elements/transmute.h"
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
#include "core/compiler/session.h"

namespace gfx::compiler {
std::unordered_map<ast_node_types_t, node_evaluator_callable> ast_evaluator::s_node_evaluators = {
    {ast_node_types_t::pair,                    std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::label,                   std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::module,                  std::bind(&ast_evaluator::module, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::symbol,                  std::bind(&ast_evaluator::symbol, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::type_list,               std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::proc_call,               std::bind(&ast_evaluator::proc_call, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::statement,               std::bind(&ast_evaluator::statement, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::attribute,               std::bind(&ast_evaluator::attribute, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::directive,               std::bind(&ast_evaluator::directive, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::assignment,              std::bind(&ast_evaluator::assignment, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::expression,              std::bind(&ast_evaluator::expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::label_list,              std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::basic_block,             std::bind(&ast_evaluator::basic_block, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::symbol_part,             std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::line_comment,            std::bind(&ast_evaluator::line_comment, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::null_literal,            std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::block_comment,           std::bind(&ast_evaluator::block_comment, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::argument_list,           std::bind(&ast_evaluator::argument_list, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::if_expression,           std::bind(&ast_evaluator::if_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::parameter_list,          std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::number_literal,          std::bind(&ast_evaluator::number_literal, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::string_literal,          std::bind(&ast_evaluator::string_literal, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::unary_operator,          std::bind(&ast_evaluator::unary_operator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::cast_expression,         std::bind(&ast_evaluator::cast_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::from_expression,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::proc_expression,         std::bind(&ast_evaluator::proc_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::enum_expression,         std::bind(&ast_evaluator::enum_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::binary_operator,         std::bind(&ast_evaluator::binary_operator, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::boolean_literal,         std::bind(&ast_evaluator::boolean_literal, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::map_constructor,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::else_expression,         std::bind(&ast_evaluator::else_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::while_statement,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::break_statement,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::with_expression,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::type_identifier,         std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::alias_expression,        std::bind(&ast_evaluator::alias_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::defer_expression,        std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::union_expression,        std::bind(&ast_evaluator::union_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::return_statement,        std::bind(&ast_evaluator::return_statement, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::symbol_reference,        std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::extend_statement,        std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::for_in_statement,        std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::switch_expression,       std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::import_expression,       std::bind(&ast_evaluator::import_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::struct_expression,       std::bind(&ast_evaluator::struct_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::character_literal,       std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::array_constructor,       std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::module_expression,       std::bind(&ast_evaluator::module_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::elseif_expression,       std::bind(&ast_evaluator::if_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::continue_statement,      std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::constant_expression,     std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::namespace_expression,    std::bind(&ast_evaluator::namespace_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::subscript_expression,    std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::return_argument_list,    std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::array_subscript_list,    std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::assignment_source_list,  std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::assignment_target_list,  std::bind(&ast_evaluator::noop, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
    {ast_node_types_t::transmute_expression,    std::bind(&ast_evaluator::transmute_expression, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
};


ast_evaluator::ast_evaluator(compiler::session& session)
    : session_(session)
{

}

element *ast_evaluator::evaluate_in_scope(const evaluator_context_t ast_context, const ast_node_t *node,
                                    compiler::block* scope, element_type_t default_block_type)
{
    session_.scope_manager().push_scope(scope);
    auto result = evaluate(node, default_block_type);
    session_.scope_manager().pop_scope();
    return result;
}

element* ast_evaluator::evaluate(const ast_node_t *node, element_type_t default_block_type)
{
    if (node ==nullptr) {
        return nullptr;
    }

    compiler::evaluator_context_t context(session_);
    context.node = node;
    context.scope = session_.scope_manager().current_scope();
    context.default_block_type = default_block_type;

    auto it = s_node_evaluators.find(node->type);
    if (it != s_node_evaluators.end()) {
        evaluator_result_t result {};
        if (it->second(this, context, result)) {
            return result.element;
        } else {
            session_.error("P071", fmt::format("ast node evaluation failed: id = {}, type = {}",node->id,
                    ast_node_type_name(node->type)), node->location);
        }
    }

    return nullptr;
}

void ast_evaluator::apply_attributes(const evaluator_context_t ast_context, compiler::element* element, const ast_node_t *node)
{
    if (node == nullptr) {
        return;
    }
    for (auto it = node->children.begin(); it != node->children.end(); ++it) {
        const auto& child_node = *it;
        if (child_node->type == ast_node_types_t::attribute) {
            auto attr = dynamic_cast<compiler::attribute*>(evaluate(child_node.get()));
            attr->parent_element(element);
            auto &attributes = element->attributes();
            attributes.add(attr);
        }
    }
}

identifier* ast_evaluator::add_identifier_to_scope(const evaluator_context_t context, symbol_element *symbol, type_find_result_t& type_find_result,
                                             const ast_node_t *node, size_t source_index, compiler::block* parent_scope)
{
    auto &builder_ = session_.builder();

    auto scope = symbol->is_qualified() ? session_.scope_manager().current_top_level()
        : (parent_scope != nullptr ? parent_scope : session_.scope_manager().current_scope());

    scope = add_namespaces_to_scope(context, node, symbol, scope);

    ast_node_shared_ptr source_node = nullptr;
    if (node != nullptr) {
        source_node = node->rhs->children[source_index];
    }

    compiler::element* init_expr = nullptr;
    compiler::initializer * init = nullptr;
    if (node != nullptr) {
        init_expr = evaluate_in_scope(context, source_node.get(), scope);
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
    apply_attributes(context, new_identifier, node);
    if (init_expr != nullptr) {
        if (init ==nullptr) {
            init_expr->parent_element(new_identifier);
        } else {
            auto folded_expr = init_expr->fold(session_);
            if (folded_expr != nullptr) {
                init_expr = folded_expr;
                auto old_expr = init->expression();
                init->expression(init_expr);
                session_.elements().remove(old_expr->id());
            }
        }
    }
    if (type_find_result.type == nullptr) {
        if (init_expr != nullptr) {
            type_find_result.type = init_expr->infer_type(session_);
            new_identifier->type(type_find_result.type);
            new_identifier->inferred_type(type_find_result.type != nullptr);
        }
        if (type_find_result.type == nullptr) {
            new_identifier->type(session_.scope_manager().unknown_type_from_result(scope, new_identifier, type_find_result));
        }
    } else {
        new_identifier->type(type_find_result.type);
    }

    scope->identifiers().add(new_identifier);
    if (init != nullptr && init->expression()->element_type() == element_type_t::proc_type) {
        add_procedure_instance(context, dynamic_cast<procedure_type*>(init->expression()), source_node.get());
    }

    if (init == nullptr && init_expr == nullptr && new_identifier->type() == nullptr) {
        session_.error("P019", fmt::format("unable to infer type: {}", new_identifier->symbol()->name()),
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

void ast_evaluator::add_composite_type_fields(const evaluator_context_t context, composite_type *type, const ast_node_t *block)
{
    auto &builder_ = session_.builder();
    auto u32_type = session_.scope_manager().find_type(qualified_symbol_t{.name= "u32"});

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
                    auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(context,
                        symbol_node.get() ,type->scope()));
                    type_find_result_t type_find_result {};
                    session_.scope_manager().find_identifier_type(type_find_result, source_list->children[i], type->scope());
                    auto init = builder_.make_initializer(type->scope(), evaluate_in_scope(context,
                        source_list->children[i].get(), type->scope()));
                    auto field_identifier = builder_.make_identifier(type->scope(), symbol, init);
                    if (type_find_result.type == nullptr) {
                        type_find_result.type = init->expression()->infer_type(session_);
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
                auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(context,
                    expr_node.get(), type->scope()));
                type_find_result_t type_find_result {};
                session_.scope_manager().find_identifier_type(type_find_result, expr_node->rhs, type->scope());
                auto field_identifier = builder_.make_identifier(type->scope(), symbol, nullptr);
                if (type_find_result.type == nullptr) {
                    if (type->type() == composite_types_t::enum_type) {
                        field_identifier->type(u32_type);
                    } else {
                        field_identifier->type(session_.scope_manager().unknown_type_from_result(type->scope(), field_identifier,
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

void ast_evaluator::add_procedure_instance(const evaluator_context_t context, procedure_type *proc_type, const ast_node_t *node)
{
    auto &builder_ = session_.builder();
    if (node->children.empty()) {
        return;
    }
    for (const auto& child_node : node->children) {
        switch (child_node->type) {
            case ast_node_types_t::attribute: {
                auto attribute = builder_.make_attribute(proc_type->scope(), child_node->token.value,
                    evaluate(child_node->lhs.get()));
                attribute->parent_element(proc_type);
                proc_type->attributes().add(attribute);
                break;
            }
            case ast_node_types_t::basic_block: {
                auto basic_block = dynamic_cast<compiler::block*>(evaluate_in_scope(context, child_node.get(),
                    proc_type->scope(), element_type_t::proc_instance_block));
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
            auto attr = dynamic_cast<compiler::attribute*>(expr);
            scope->attributes().add(attr);
            break;
        }
        case element_type_t::statement: {
            auto state = dynamic_cast<compiler::statement*>(expr);
            scope->statements().emplace_back(state);
            break;
        }
        default:
            break;
    }
}
compiler::element* ast_evaluator::resolve_symbol_or_evaluate(const evaluator_context_t& context, const ast_node_t *node)
{
    auto &builder_ = session_.builder();
    compiler::element* element = nullptr;
    if (node != nullptr && node->type == ast_node_types_t::symbol) {
        qualified_symbol_t qualified_symbol {};
        gfx::compiler::element_builder::make_qualified_symbol(qualified_symbol, node);
        element = builder_.make_identifier_reference(session_.scope_manager().current_scope(), qualified_symbol,
                                                     session_.scope_manager().find_identifier(qualified_symbol));
    } else {
        element = evaluate(node);
    }
    return element;
}

compiler::block* ast_evaluator::add_namespaces_to_scope(const evaluator_context_t& context,
    const ast_node_t *node, compiler::symbol_element* symbol,compiler::block* parent_scope)
{
    auto &builder_ = session_.builder();
    auto namespace_type = session_.scope_manager().find_type(qualified_symbol_t {
        .name = "namespace"
    });

    auto namespaces = symbol->namespaces();
    auto scope = parent_scope;
    string_list_t temp_list {};
    std::string namespace_name {};
    for (size_t i = 0; i < namespaces.size(); i++) {
        if (!namespace_name.empty()) {
            temp_list.push_back(namespace_name);
        }
        namespace_name = namespaces[i];
        auto var = scope->identifiers().find(namespace_name);
        if (var == nullptr) {
            auto new_scope = builder_.make_block(scope, element_type_t::block);
            auto ns = builder_.make_namespace(scope, new_scope);
            auto ns_identifier = builder_.make_identifier(scope,
                builder_.make_symbol(scope, namespace_name, temp_list),
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
                session_.error("P018", "only a namespace is valid within a qualified name.",
                    node->lhs->location);
                return nullptr;
            }
        }
    }
    return scope;
}

bool ast_evaluator::noop(evaluator_context_t& context, evaluator_result_t& result)
{
    return false;
}

bool ast_evaluator::symbol(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_symbol_from_node(context.node);
    return true;
}

bool ast_evaluator::attribute(evaluator_context_t& context, evaluator_result_t& result) 
{
    result.element = session_.builder().make_attribute(session_.scope_manager().current_scope(), context.node->token.value,
        evaluate(context.node->lhs.get()));
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::directive(evaluator_context_t& context, evaluator_result_t& result)
{
    auto expression = evaluate(context.node->lhs.get());
    auto directive_element = session_.builder().make_directive(session_.scope_manager().current_scope(),
                   context.node->token.value, expression);
    directive_element->location(context.node->location);
    apply_attributes(context.session, directive_element, context.node);
    directive_element->evaluate(context.session);
    result.element = directive_element;
    return true;
}

bool ast_evaluator::module(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto &program_= session_.program();
    auto &top_level_stack = session_.scope_manager().top_level_stack();
    auto module_block = builder_.make_block(program_.block(), element_type_t::module_block);
    auto module = builder_.make_module(program_.block(), module_block);
    module->source_file(context.session.current_source_file());
    program_.block()->blocks().push_back(module_block);

    session_.scope_manager().push_scope(module_block);
    top_level_stack.push(module_block);
    session_.scope_manager().module_stack().push(module);

    for (auto it = context.node->children.begin();
         it != context.node->children.end();
         ++it) {
        auto expr = evaluate((*it).get(), context.default_block_type);
        if (expr == nullptr) {
            return false;
        }

        add_expression_to_scope(module_block, expr);
        expr->parent_element(module);
    }
    top_level_stack.pop();
    result.element = module;

    return true;
}

bool ast_evaluator::module_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto expr = resolve_symbol_or_evaluate(context, context.node->rhs.get());
    auto reference = builder_.make_module_reference(session_.scope_manager().current_scope(), expr);

    std::string path;
    if (expr->is_constant() && expr->as_string(path)) {
        std::filesystem::path source_path(path);
        auto current_source_file = context.session.current_source_file();
        if (current_source_file != nullptr
            &&  source_path.is_relative()) {
            auto absolutePath = current_source_file->path().parent_path() / source_path;
            source_path = absolutePath;
        }
        auto source_file = context.session.add_source_file(source_path);
        auto module = session_.compile_module(source_file);
        if (module == nullptr) {
            context.session.error("C021", "unable to load module.",context.node->rhs->location);
            return false;
        }
        reference->module(module);
        result.element = reference;
    } else {
        context.session.error("C021", "expected string literal or constant string variable.",
            context.node->rhs->location);
        return false;
    }

    return true;
}

bool ast_evaluator::line_comment(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_comment(session_.scope_manager().current_scope(),
        comment_type_t::line, context.node->token.value);
    return true;
}

bool ast_evaluator::block_comment(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_comment(session_.scope_manager().current_scope(),
        comment_type_t::block, context.node->token.value);
    return true;
}

bool ast_evaluator::string_literal(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_string(session_.scope_manager().current_scope(),
                                                    context.node->token.value);
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::number_literal(evaluator_context_t& context, evaluator_result_t& result) {
    auto &builder_ =session_.builder();
    switch (context.node->token.number_type) {
        case number_types_t::integer: {
            uint64_t value;
            if (context.node->token.parse(value) == conversion_result_t::success) {
                if (context.node->token.is_signed()) {
                    result.element = builder_.make_integer(session_.scope_manager().current_scope(),
                        twos_complement(value));
                } else {
                    result.element = builder_.make_integer(session_.scope_manager().current_scope(), value);
                }
                result.element->location(context.node->location);
                return true;
            } else {
                context.session.error("P041", "invalid integer literal", context.node->location);
            }
            break;
        }
        case number_types_t::floating_point: {
            double value;
            if (context.node->token.parse(value) == conversion_result_t::success) {
                result.element = builder_.make_float(session_.scope_manager().current_scope(),
                    value);
                result.element->location(context.node->location);
                return true;
            } else {
                context.session.error("P041", "invalid float literal", context.node->location);
            }
            break;
        }
        default:
            break;
    }

    return false;
}

bool ast_evaluator::boolean_literal(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_bool(session_.scope_manager().current_scope(),
                                                  context.node->token.as_bool());
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::namespace_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = session_.builder().make_namespace(session_.scope_manager().current_scope(),
        evaluate(context.node->rhs.get(), context.default_block_type));
    return true;
}

bool ast_evaluator::expression(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element =session_.builder().make_expression(session_.scope_manager().current_scope(),
        evaluate(context.node->lhs.get()));
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::argument_list(evaluator_context_t& context, evaluator_result_t& result)
{
    auto args =session_.builder().make_argument_list(session_.scope_manager().current_scope());
    for (const auto& arg_node : context.node->children) {
        auto arg = resolve_symbol_or_evaluate(context, arg_node.get());
        args->add(arg);
        arg->parent_element(args);
    }
    result.element = args;
    return true;
}

bool ast_evaluator::unary_operator(evaluator_context_t& context, evaluator_result_t& result)
{
    auto it = s_unary_operators.find(context.node->token.type);
    if (it == s_unary_operators.end()) {
        return false;
    }

    result.element =session_.builder().make_unary_operator(session_.scope_manager().current_scope(),
        it->second, resolve_symbol_or_evaluate(context, context.node->rhs.get()));
    return true;
}

bool ast_evaluator::binary_operator(evaluator_context_t& context, evaluator_result_t& result)
{
    auto it = s_binary_operators.find(context.node->token.type);
    if (it == s_binary_operators.end()) {
        return false;
    }

    auto lhs = resolve_symbol_or_evaluate(context, context.node->lhs.get());
    auto rhs = resolve_symbol_or_evaluate(context, context.node->rhs.get());
    result.element = session_.builder().make_binary_operator(session_.scope_manager().current_scope(),
        it->second, lhs, rhs);
    return true;
}

bool ast_evaluator::cast_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto type_name = context.node->lhs->lhs->children[0]->token.value;
    auto type = session_.scope_manager().find_type(qualified_symbol_t {.name = type_name});
    if (type == nullptr) {
        context.session.error("P002", fmt::format("unknown type '{}'.", type_name),
            context.node->lhs->lhs->location);
        return false;
    }
    result.element = session_.builder().make_cast(session_.scope_manager().current_scope(),type,
        resolve_symbol_or_evaluate(context, context.node->rhs.get()));
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::alias_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element =session_.builder().make_alias(session_.scope_manager().current_scope(),
        resolve_symbol_or_evaluate(context, context.node->lhs.get()));
    result.element->location(context.node->location);
    return true;
}

bool ast_evaluator::return_statement(evaluator_context_t& context, evaluator_result_t& result)
{
    auto return_element = session_.builder().make_return(session_.scope_manager().current_scope());
    auto& expressions = return_element->expressions();
    for (const auto& arg_node : context.node->rhs->children) {
        auto arg = resolve_symbol_or_evaluate(context, arg_node.get());
        expressions.push_back(arg);
        arg->parent_element(return_element);
    }
    result.element = return_element;
    return true;
}

bool ast_evaluator::import_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ =session_.builder();
    qualified_symbol_t qualified_symbol {};
    compiler::element_builder::make_qualified_symbol(qualified_symbol, context.node->lhs.get());

    compiler::identifier_reference* from_reference = nullptr;
    if (context.node->rhs != nullptr) {
        from_reference = dynamic_cast<compiler::identifier_reference*>(
            resolve_symbol_or_evaluate(context, context.node->rhs.get()));
        qualified_symbol.namespaces.insert(qualified_symbol.namespaces.begin(),
            from_reference->symbol().name);
    }

    auto identifier_reference = builder_.make_identifier_reference(session_.scope_manager().current_scope(),
        qualified_symbol, session_.scope_manager().find_identifier(qualified_symbol));
    auto import = builder_.make_import(session_.scope_manager().current_scope(),
        identifier_reference, from_reference,
        dynamic_cast<compiler::module*>(session_.scope_manager().current_top_level()->parent_element()));
    add_expression_to_scope(session_.scope_manager().current_scope(), import);

    result.element = import;

    return true;
}

bool ast_evaluator::basic_block(evaluator_context_t& context, evaluator_result_t& result)
{
    auto active_scope = session_.scope_manager().push_new_block(context.default_block_type);

    for (auto it = context.node->children.begin();
         it != context.node->children.end();
         ++it) {
        const auto& current_node = *it;
        auto expr = evaluate(current_node.get(), context.default_block_type);
        if (expr == nullptr) {
            context.session.error("C024", "invalid statement", current_node->location);
            return false;
        }
        add_expression_to_scope(active_scope, expr);
        expr->parent_element(active_scope);
    }

    result.element = session_.scope_manager().pop_scope();
    return true;
}

bool ast_evaluator::proc_call(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    qualified_symbol_t qualified_symbol {};
    compiler::element_builder::make_qualified_symbol(qualified_symbol, context.node->lhs.get());
    auto proc_identifier = session_.scope_manager().find_identifier(qualified_symbol);

    compiler::argument_list* args = nullptr;
    auto expr = evaluate(context.node->rhs.get());
    if (expr != nullptr) {
        args = dynamic_cast<compiler::argument_list*>(expr);
    }
    result.element = builder_.make_procedure_call(session_.scope_manager().current_scope(),
        builder_.make_identifier_reference(session_.scope_manager().current_scope(),
            qualified_symbol,
            proc_identifier),
        args);
    result.element->location(context.node->location);

    return true;
}

bool ast_evaluator::statement(evaluator_context_t& context, evaluator_result_t& result)
{
    label_list_t labels {};
    auto &builder_ = session_.builder();
    if (context.node->lhs != nullptr) {
        for (const auto& label : context.node->lhs->children) {
            labels.push_back(builder_.make_label(session_.scope_manager().current_scope(),
                label->token.value));
        }
    }

    auto expr = evaluate(context.node->rhs.get());
    if (expr == nullptr) {
        return false;
    }

    if (expr->element_type() == element_type_t::symbol) {
        type_find_result_t find_type_result {};
        session_.scope_manager().find_identifier_type(find_type_result, context.node->rhs->rhs);
        expr = add_identifier_to_scope(
            context.session,
            dynamic_cast<compiler::symbol_element*>(expr),
            find_type_result,
            nullptr,
            0);
    }

    result.element = builder_.make_statement(session_.scope_manager().current_scope(),
        labels, expr);

    return true;
}

bool ast_evaluator::enum_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto active_scope = session_.scope_manager().current_scope();
    auto enum_type = builder_.make_enum_type(active_scope,
        builder_.make_block(active_scope, element_type_t::block));
    active_scope->types().add(enum_type);
    add_composite_type_fields(context.session, enum_type, context.node->rhs.get());
    if (!enum_type->initialize(context.session)) {
        return false;
    }
    result.element = enum_type;
    return true;
}

bool ast_evaluator::struct_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto active_scope = session_.scope_manager().current_scope();
    auto struct_type = builder_.make_struct_type(active_scope,
       builder_.make_block(active_scope, element_type_t::block));
    active_scope->types().add(struct_type);
    add_composite_type_fields(context.session, struct_type, context.node->rhs.get());
    if (!struct_type->initialize(context.session)) {
        return false;
    }
    result.element = struct_type;
    return true;
}

bool ast_evaluator::union_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto active_scope = session_.scope_manager().current_scope();
    auto union_type = builder_.make_union_type(active_scope,
        builder_.make_block(active_scope, element_type_t::block));
    active_scope->types().add(union_type);
    add_composite_type_fields(context.session, union_type,context.node->rhs.get());
    if (!union_type->initialize(context.session)) {
        return false;
    }
    result.element = union_type;
    return true;
}

bool ast_evaluator::else_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    result.element = evaluate(context.node->children[0].get());
    return true;
}

bool ast_evaluator::if_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto predicate = evaluate(context.node->lhs.get());
    auto true_branch = evaluate(context.node->children[0].get());
    auto false_branch = evaluate(context.node->rhs.get());
    result.element = builder_.make_if(session_.scope_manager().current_scope(), predicate, true_branch, false_branch);
    return true;
}

bool ast_evaluator::proc_expression(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    auto active_scope = session_.scope_manager().current_scope();
    auto block_scope = builder_.make_block(active_scope, element_type_t::proc_type_block);
    auto proc_type = builder_.make_procedure_type(active_scope, block_scope);
    active_scope->types().add(proc_type);
    auto count = 0;
    for (const auto& type_node : context.node->lhs->children) {
        switch (type_node->type) {
            case ast_node_types_t::symbol: {
                auto return_identifier = builder_.make_identifier(block_scope,
                    builder_.make_symbol(block_scope, fmt::format("_{}", count++)),
                    nullptr);
                return_identifier->usage(identifier_usage_t::stack);
                return_identifier->type(session_.scope_manager().find_type(qualified_symbol_t {
                    .name = type_node->children[0]->token.value
                }));
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

    for (const auto& param_node : context.node->rhs->children) {
        switch (param_node->type) {
            case ast_node_types_t::assignment: {
                // XXX: in the parameter list, multiple targets is an error
                const auto& first_target = param_node->lhs->children[0];
                auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(
                    context.session, first_target.get(), block_scope));
                type_find_result_t find_type_result {};
                session_.scope_manager().find_identifier_type(find_type_result, first_target->rhs, block_scope);
                auto param_identifier = add_identifier_to_scope(context.session, symbol,
                    find_type_result, param_node.get(), 0, block_scope);
                param_identifier->usage(identifier_usage_t::stack);
                auto field = builder_.make_field(block_scope, param_identifier);
                proc_type->parameters().add(field);
                field->parent_element(proc_type);
                break;
            }
            case ast_node_types_t::symbol: {
                auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate_in_scope(
                    context.session, param_node.get(), block_scope));
                type_find_result_t find_type_result {};
                session_.scope_manager().find_identifier_type(find_type_result, param_node->rhs, block_scope);
                auto param_identifier = add_identifier_to_scope(context.session, symbol,
                    find_type_result, nullptr, 0, block_scope);
                if (param_identifier != nullptr) {
                    param_identifier->usage(identifier_usage_t::stack);
                    auto field = builder_.make_field(block_scope, param_identifier);
                    proc_type->parameters().add(field);
                    field->parent_element(proc_type);
                } else {
                    context.session.error("P014", fmt::format("invalid parameter declaration: {}", symbol->name()),
                        symbol->location());
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    result.element = proc_type;

    return true;
}

bool ast_evaluator::assignment(evaluator_context_t& context, evaluator_result_t& result)
{
    auto &builder_ = session_.builder();
    const auto& target_list = context.node->lhs;
    const auto& source_list = context.node->rhs;

    if (target_list->children.size() != source_list->children.size()) {
        context.session.error("P027", "the number of left-hand-side targets must match"
            " the number of right-hand-side expressions.",
            source_list->location);
        return false;
    }

    element_list_t list {};

    for (size_t i = 0; i < target_list->children.size(); i++) {
        const auto& symbol_node = target_list->children[i];

        qualified_symbol_t qualified_symbol {};
        gfx::compiler::element_builder::make_qualified_symbol(qualified_symbol, symbol_node.get());
        auto existing_identifier = session_.scope_manager().find_identifier(qualified_symbol);
        if (existing_identifier != nullptr) {
            auto rhs = evaluate(source_list->children[i].get());
            if (rhs == nullptr) {
                return false;
            }
            auto binary_op = builder_.make_binary_operator(session_.scope_manager().current_scope(),
                operator_type_t::assignment, existing_identifier, rhs);
            apply_attributes(context, binary_op, context.node);
            list.emplace_back(binary_op);
        } else {
            auto symbol = dynamic_cast<compiler::symbol_element*>(evaluate(symbol_node.get()));
            type_find_result_t find_type_result {};
            session_.scope_manager().find_identifier_type(find_type_result, symbol_node->rhs);
            auto new_identifier = add_identifier_to_scope(context.session, symbol, find_type_result, context.node, i);
            if (new_identifier == nullptr) {
                return false;
            }
            list.emplace_back(new_identifier);
        }
    }

    result.element = list.front();

    return true;
}

bool ast_evaluator::transmute_expression(evaluator_context_t &context, evaluator_result_t &result)
{
    auto type_name = context.node->lhs->lhs->children[0]->token.value;
    auto type = session_.scope_manager().find_type(qualified_symbol_t {.name = type_name});
    if (type == nullptr) {
        session_.error("P002", fmt::format("unknown type '{}'.", type_name),
            context.node->lhs->lhs->location);
        return false;
    }
    result.element = session_.builder().make_transmute(session_.scope_manager().current_scope(),type,
        resolve_symbol_or_evaluate(context, context.node->rhs.get()));

    result.element->location(context.node->location);
    return true;
}

}