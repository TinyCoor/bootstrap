//
// Created by 12132 on 2022/10/6.
//

#include "element_builder.h"
#include "elements/program.h"
#include "elements/cast.h"
#include "elements/label.h"
#include "elements/module.h"
#include "elements/alias.h"
#include "elements/types/type.h"
#include "elements/import.h"
#include "elements/comment.h"
#include "elements/types/any_type.h"
#include "elements/types/bool_type.h"
#include "elements/types/tuple_type.h"
#include "elements/types/array_type.h"
#include "elements/if_element.h"
#include "elements/expression.h"
#include "elements/attribute.h"
#include "elements/identifier.h"
#include "elements/directive.h"
#include "elements/statement.h"
#include "elements/types/type_info.h"
#include "elements/types/string_type.h"
#include "elements/types/numeric_type.h"
#include "elements/types/procedure_type.h"
#include "elements/types/unknown_type.h"
#include "elements/types/pointer_type.h"
#include "elements/types/module_type.h"
#include "elements/argument_list.h"
#include "elements/boolean_literal.h"
#include "elements/string_literal.h"
#include "elements/float_literal.h"
#include "elements/integer_literal.h"
#include "elements/return_element.h"
#include "elements/unary_operator.h"
#include "elements/symbol_element.h"
#include "elements/types/composite_type.h"
#include "elements/procedure_call.h"
#include "elements/binary_operator.h"
#include "elements/namespace_element.h"
#include "elements/types/namespace_type.h"
#include "elements/procedure_instance.h"
#include "elements/identifier_reference.h"
#include "elements/module_reference.h"
#include <fmt/format.h>
namespace gfx::compiler {
element_builder::element_builder(program* program)
    : program_(program)
{

}
element_builder::~element_builder()
{

}


module_type *element_builder::make_module_type(gfx::result &r, compiler::block *parent_scope, compiler::block *scope)
{
    auto type = new module_type(parent_scope, scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

tuple_type *element_builder::make_tuple_type(result &r, compiler::block *parent_scope, compiler::block* scope)
{
    auto type = new tuple_type(parent_scope, scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

import* element_builder::make_import(compiler::block* parent_scope, element* expr,
    element* from_expr, compiler::module* module)
{
    auto import_element = new compiler::import(parent_scope, expr, from_expr, module);

    if (expr !=nullptr) {
        expr->parent_element(import_element);
    }
    if (from_expr !=nullptr) {
        from_expr->parent_element(import_element);
    }
    program_->elements().add(import_element);
    return import_element;
}

compiler::symbol_element* element_builder::make_symbol(compiler::block* parent_scope, const std::string& name,
                                               const string_list_t& namespaces)
{
    auto symbol = new compiler::symbol_element(parent_scope, name, namespaces);

    symbol->cache_fully_qualified_name();
    program_->elements().add(symbol);
    return symbol;
}

type_info *element_builder::make_type_info_type(result &r, compiler::block *parent_scope, compiler::block* scope)
{
    auto type = new type_info(parent_scope, scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

identifier_reference *element_builder::make_identifier_reference(compiler::block *parent_scope,
     const qualified_symbol_t &symbol, compiler::identifier *identifier)
{
    auto reference = new compiler::identifier_reference(parent_scope, symbol, identifier);

    if (!reference->resolved()) {
       program_->unresolved_identifier_references_.emplace_back(reference);
    }
    reference->location(symbol.location);
    program_->elements().add(reference);
    return reference;
}

module *element_builder::make_module(compiler::block* parent_scope, compiler::block* scope)
{
    auto module_element = new compiler::module(parent_scope, scope);
    if (scope !=nullptr) {
        scope->parent_element(module_element);
    }
    program_->elements().add(module_element);
    return module_element;
}

module_reference *element_builder::make_module_reference(compiler::block *parent_scope, compiler::element *expr)
{
    auto module_ref= new compiler::module_reference(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(module_ref);
    }
    program_->elements().add(module_ref);
    return module_ref;
}

bool_type *element_builder::make_bool_type(result &r, compiler::block *parent_scope)
{
    auto type = new compiler::bool_type(parent_scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
   program_->elements().add(type);
    return type;
}

array_type *element_builder::make_array_type(result &r, compiler::block* parent_scope, compiler::block* scope,
                                     compiler::type *entry_type, size_t size)
{
    auto type = new array_type(parent_scope, scope, entry_type, size);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}


namespace_type *element_builder::make_namespace_type(result &r, compiler::block* parent_scope)
{
    auto ns_type = new namespace_type(parent_scope);
    if (!ns_type->initialize(r, program_)) {
        return nullptr;
    }
    program_->elements().add(ns_type);
    return ns_type;
}

unknown_type *element_builder::make_unknown_type(result &r, compiler::block *parent_scope, symbol_element* symbol,
                                         bool is_pointer, bool is_array, size_t array_size)
{
    auto type = new compiler::unknown_type(parent_scope, symbol);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }

    type->is_array(is_array);
    type->array_size(array_size);
    type->is_pointer(is_pointer);
    symbol->parent_element(type);
    program_->elements().add(type);
    return type;
}


pointer_type *element_builder::make_pointer_type(result &r, compiler::block *parent_scope, compiler::type *base_type)
{
    auto type = new compiler::pointer_type(parent_scope, base_type);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    program_->elements().add(type);
    return type;
}


return_element *element_builder::make_return(compiler::block* parent_scope)
{
    auto return_element = new compiler::return_element(parent_scope);
    program_->elements().add(return_element);
    return return_element;
}

argument_list* element_builder::make_argument_list(compiler::block* parent_scope)
{
    auto arg = new compiler::argument_list(parent_scope);
    program_->elements().add(arg);
    return arg;
}

procedure_call *element_builder::make_procedure_call(compiler::block* parent_scope, compiler::identifier_reference* reference,
                                             compiler::argument_list* args)
{
    auto call = new compiler::procedure_call(parent_scope, reference, args);
    args->parent_element(call);
    reference->parent_element(call);
    program_->elements().add(call);
    return call;
}

namespace_element *element_builder::make_namespace(compiler::block* parent_scope, element *expr)
{
    auto ns = new compiler::namespace_element(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(ns);
    }
    program_->elements().add(ns);
    return ns;
}

class block *element_builder::make_block(compiler::block* parent_scope, element_type_t block_type)
{
    auto block = new compiler::block(parent_scope, block_type);
    program_->elements().add(block);
    return block;
}

boolean_literal *element_builder::make_bool(compiler::block* parent_scope, bool value)
{
    auto bool_literal = new compiler::boolean_literal(parent_scope, value);
    program_->elements().add(bool_literal);
    return bool_literal;
}

float_literal *element_builder::make_float(compiler::block* parent_scope, double value)
{
    auto float_literal = new compiler::float_literal(parent_scope, value);
    program_->elements().add(float_literal);
    return float_literal;
}

integer_literal *element_builder::make_integer(compiler::block* parent_scope, uint64_t value)
{
    auto integer_literal = new compiler::integer_literal(parent_scope, value);
    program_->elements().add(integer_literal);
    return integer_literal;
}

string_literal *element_builder::make_string(compiler::block* parent_scope, const std::string &value)
{
    auto string_literal = new compiler::string_literal(parent_scope, value);

    auto it = program_->interned_string_literals_.find(value);
    if (it != program_->interned_string_literals_.end()) {
        auto& list = it->second;
        list.emplace_back(string_literal);
    } else {
        string_literal_list_t list {};
        list.emplace_back(string_literal);
        program_->interned_string_literals_.insert(std::make_pair(value, list));
    }
    program_->elements().add(string_literal);
    return string_literal;
}

field* element_builder::make_field(compiler::block* parent_scope, compiler::identifier* identifier)
{
    auto field = new compiler::field(parent_scope, identifier);
    identifier->parent_element(field);
    program_->elements().add(field);
    return field;
}

attribute *element_builder::make_attribute(compiler::block* parent_scope, const std::string &name, element *expr)
{
    auto attr = new compiler::attribute(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(attr);
    }
    program_->elements().add(attr);
    return attr;
}

composite_type* element_builder::make_enum_type(result &r, compiler::block* parent_block, compiler::block* scope)
{
    std::string name = fmt::format("__enum_{}__", id_pool::instance()->allocate());
    auto symbol =  make_symbol(parent_block, name);
    auto type = new composite_type(parent_block, composite_types_t::enum_type, scope, symbol);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    symbol->parent_element(type);
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

composite_type* element_builder::make_union_type(result& r, compiler::block* parent_block, compiler::block* scope)
{
    std::string name = fmt::format("__union_{}__", id_pool::instance()->allocate());
    auto symbol = make_symbol(parent_block, name);
    auto type = new composite_type(parent_block,  composite_types_t::union_type, scope, symbol);
    symbol->parent_element(type);
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

composite_type* element_builder::make_struct_type(result& r, compiler::block* parent_scope, compiler::block* scope)
{
    std::string name = fmt::format("__struct_{}__", id_pool::instance()->allocate());
    auto symbol = make_symbol(parent_scope, name);
    auto type = new composite_type(parent_scope, composite_types_t::struct_type, scope, symbol);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    symbol->parent_element(type);
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

comment *element_builder::make_comment(compiler::block* parent_scope, comment_type_t type, const std::string &value)
{
    auto commnet = new compiler::comment(parent_scope, type, value);
    program_->elements().add(commnet);
    return commnet;
}

directive *element_builder::make_directive(compiler::block* parent_scope, const std::string &name, element *expr)
{
    auto directive = new compiler::directive(parent_scope, name, expr);
    if (expr != nullptr) {
        expr->parent_element(directive);
    }
    program_->elements().add(directive);
    return directive;
}

statement *element_builder::make_statement(compiler::block* parent_scope, const label_list_t &labels, element *expr)
{
    auto statement = new compiler::statement(parent_scope, expr);
    if (expr != nullptr && expr->parent_element() == nullptr) {
        expr->parent_element(statement);
    }

    for (auto &label : labels) {
        statement->labels().push_back(label);
        label->parent_element(statement);
    }
    program_->elements().add(statement);
    return statement;
}

label *element_builder::make_label(compiler::block* parent_scope, const std::string &name)
{
    auto label = new compiler::label(parent_scope, name);
    program_->elements().add(label);
    return label;
}

identifier* element_builder::make_identifier(compiler::block* parent_scope, compiler::symbol_element* symbol, initializer* expr)
{
    auto identifier = new compiler::identifier(parent_scope, symbol, expr);
    if (expr != nullptr) {
        expr->parent_element(identifier);
    }
    symbol->parent_element(identifier);
    identifier->location(symbol->location());
    program_->elements().add(identifier);
    return identifier;
}

unary_operator* element_builder::make_unary_operator(compiler::block* parent_scope, operator_type_t type, element* rhs)
{
    auto unary_operator = new compiler::unary_operator(parent_scope, type, rhs);
    rhs->parent_element(unary_operator);
    program_->elements().add(unary_operator);
    return unary_operator;
}

binary_operator* element_builder::make_binary_operator(compiler::block* parent_scope, operator_type_t type, element* lhs, element* rhs)
{
    auto binary_operator = new compiler::binary_operator(parent_scope, type, lhs, rhs);
    lhs->parent_element(binary_operator);
    rhs->parent_element(binary_operator);
    program_->elements().add(binary_operator);
    return binary_operator;
}

expression* element_builder::make_expression(compiler::block* parent_scope, element* expr)
{
    auto expression = new compiler::expression(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(expression);
    }
    program_->elements().add(expression);
    return expression;
}

alias *element_builder::make_alias(compiler::block* parent_scope, element *expr)
{
    auto alias_type = new compiler::alias(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(alias_type);
    }
    program_->elements().add(alias_type);
    return alias_type;
}

procedure_type* element_builder::make_procedure_type(result &r, compiler::block* parent_scope, compiler::block* block_scope)
{
    // XXX: the name of the proc isn't correct here, but it works temporarily.
    std::string name = fmt::format("__proc_{}__", id_pool::instance()->allocate());
    auto procedure_type = new compiler::procedure_type(parent_scope, block_scope, make_symbol(parent_scope, name));
    if (block_scope != nullptr) {
        block_scope->parent_element(procedure_type);
    }
    program_->elements().add(procedure_type);
    return procedure_type;
}

procedure_instance* element_builder::make_procedure_instance(compiler::block* parent_scope,
                                                     compiler::type* procedure_type, compiler::block* scope)
{
    auto procedure_instance = new compiler::procedure_instance(parent_scope, procedure_type, scope);
    scope->parent_element(procedure_instance);
    program_->elements().add(procedure_instance);
    return procedure_instance;
}

initializer* element_builder::make_initializer(compiler::block* parent_scope, element* expr)
{
    auto initializer = new compiler::initializer(parent_scope, expr);
    if (expr != nullptr) {
        expr->parent_element(initializer);
    }
    program_->elements().add(initializer);
    return initializer;
}

cast* element_builder::make_cast(compiler::block* parent_scope, compiler::type* type, element* expr)
{
    auto cast = new compiler::cast(parent_scope, type, expr);
    program_->elements().add(cast);
    if (expr != nullptr) {
        expr->parent_element(cast);
    }
    return cast;
}

if_element *element_builder::make_if(compiler::block* parent_scope, element *predicate,
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
    program_->elements().add(if_element);
    return if_element;
}

any_type *element_builder::make_any_type(result &r, compiler::block* parent_scope, compiler::block* scope)
{
    auto type = new any_type(parent_scope, scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

string_type *element_builder::make_string_type(result &r, compiler::block* parent_scope, compiler::block* scope)
{
    auto type = new string_type(parent_scope, scope);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    scope->parent_element(type);
    program_->elements().add(type);
    return type;
}

numeric_type *element_builder::make_numeric_type(result &r, compiler::block* parent_scope, const std::string &name, int64_t min, uint64_t max,
    bool is_signed, type_number_class_t number_class)
{
    auto type = new numeric_type(parent_scope, make_symbol(parent_scope, name), min, max, is_signed, number_class);
    if (!type->initialize(r, program_)) {
        return nullptr;
    }
    program_->elements().add(type);
    return type;

}

compiler::symbol_element* element_builder::make_symbol_from_node(result& r, const ast_node_t *node)
{
    qualified_symbol_t qualified_symbol {};
    make_qualified_symbol(qualified_symbol, node);
    auto symbol = make_symbol(program_->current_scope(), qualified_symbol.name, qualified_symbol.namespaces);
    symbol->location(node->location);
    symbol->constant(node->is_constant_expression());
    return symbol;
}

void element_builder::make_qualified_symbol(qualified_symbol_t& symbol, const ast_node_t *node)
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



}