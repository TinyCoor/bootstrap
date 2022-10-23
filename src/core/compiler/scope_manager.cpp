//
// Created by 12132 on 2022/10/21.
//

#include "scope_manager.h"
#include <fmt/format.h>
#include "common/bytes.h"
#include "common/defer.h"
#include "elements/module.h"
#include "elements/cast.h"
#include "elements/label.h"
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

namespace gfx::compiler {
scope_manager::scope_manager(compiler::session &session)
    : session_(session)
{

}

compiler::block *scope_manager::current_top_level() const
{
    if (top_level_stack_.empty()) {
        return nullptr;
    }
    return top_level_stack_.top();
}

compiler::block *scope_manager::push_new_block(element_type_t type)
{
    auto &builder_= session_.builder();
    auto parent_scope = current_scope();
    auto scope_block = builder_.make_block(parent_scope, type);

    if (parent_scope != nullptr) {
        scope_block->parent_element(parent_scope);
        parent_scope->blocks().push_back(scope_block);
    }

    push_scope(scope_block);
    return scope_block;
}

void scope_manager::push_scope(class block *block)
{
    scope_stack_.push(block);
}

compiler::identifier *scope_manager::find_identifier(const qualified_symbol_t& symbol, compiler::block* scope) const
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

void scope_manager::add_type_to_scope(compiler::type *type) const
{
    auto scope = current_scope();
    scope->types().add(type);
    auto identifier = session_.builder().make_identifier(scope, type->symbol(), nullptr);
    identifier->type(type);
    scope->identifiers().add(identifier);
}

bool scope_manager::find_identifier_type(type_find_result_t &result,
    const ast_node_shared_ptr &type_node, compiler::block* parent_scope)
{
    if (type_node == nullptr) {
        return false;
    }
    compiler::element_builder::make_qualified_symbol(result.type_name, type_node->lhs.get());
    result.array_size = 0;
    result.is_array = type_node->is_array();
    result.is_spread = type_node->is_spread();
    result.is_pointer = type_node->is_pointer();
    session_.builder().make_complete_type(result, parent_scope);
    return result.type !=nullptr;
}

unknown_type *scope_manager::unknown_type_from_result(compiler::block *scope,
     compiler::identifier *identifier, type_find_result_t &result)
{
    auto symbol = session_.builder().make_symbol(scope, result.type_name.name, result.type_name.namespaces);
    auto type = session_.builder().make_unknown_type(scope, symbol, result.is_pointer, result.is_array, result.array_size);
    identifiers_with_unknown_types_.push_back(identifier);
    return type;
}

bool scope_manager::within_procedure_scope(compiler::block* parent_scope) const
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

bool scope_manager::visit_blocks(result &r, const compiler::block_visitor_callable &callable,
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

block *scope_manager::pop_scope()
{
    if (scope_stack_.empty()) {
        return nullptr;
    }
    auto top = scope_stack_.top();
    scope_stack_.pop();
    return top;
}

compiler::module *scope_manager::current_module() const
{
    if (module_stack_.empty()) {
        return nullptr;
    }
    return module_stack_.top();
}

compiler::module *scope_manager::find_module(compiler::element *element)
{
    return dynamic_cast<compiler::module*>(walk_parent_elements(element,
        [](compiler::element* each) -> compiler::element* {
          if (each->element_type() == element_type_t::module)
              return each;
          return nullptr;
        }));
}

element *scope_manager::walk_parent_scopes(compiler::block *scope, const scope_visitor_callable &callable)
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

element *scope_manager::walk_parent_elements(compiler::element *element, const element_visitor_callable &callable)
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

element *scope_manager::walk_qualified_symbol(const qualified_symbol_t &symbol, compiler::block *scope,
                                        const namespace_visitor_callable &callable) const
{
    auto block_scope = scope != nullptr ? scope : current_top_level();
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

compiler::type *scope_manager::find_pointer_type(compiler::type *base_type, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::pointer_type::name_for_pointer(base_type)
    }, scope);
}

compiler::type *scope_manager::find_array_type(compiler::type *entry_type, size_t size, compiler::block* scope) const
{
    return find_type(qualified_symbol_t {
        .name = compiler::array_type::name_for_array(entry_type, size)
    }, scope);
}

compiler::type *scope_manager::find_type(const qualified_symbol_t &symbol, compiler::block* scope) const
{
    if (symbol.is_qualified()) {
        return dynamic_cast<compiler::type*>(walk_qualified_symbol(symbol, current_top_level(),
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


compiler::type *scope_manager::make_complete_type(compiler::session& session, type_find_result_t &result, compiler::block *parent_scope)
{
    auto &builder_ = session.builder();
    result.type = find_type(result.type_name, parent_scope);
    if (result.type != nullptr) {
        if (result.is_array) {
            auto array_type = find_array_type(result.type, result.array_size, parent_scope);
            if (array_type == nullptr) {
                array_type = builder_.make_array_type(parent_scope, builder_.make_block(parent_scope, element_type_t::block),
                                                      result.type, result.array_size);
            }
            result.type = array_type;
        }

        if (result.is_pointer) {
            auto pointer_type = find_pointer_type(result.type, parent_scope);
            if (pointer_type == nullptr) {
                pointer_type = builder_.make_pointer_type(parent_scope, result.type);
            }
            result.type = pointer_type;
        }
    }
    return result.type;
}

block_stack_t &scope_manager::top_level_stack()
{
    return top_level_stack_;
}

block *scope_manager::current_scope() const
{
    if (scope_stack_.empty()) {
        return nullptr;
    }
    return scope_stack_.top();
}

identifier_list_t &scope_manager::identifiers_with_unknown_types()
{
    return identifiers_with_unknown_types_;
}

identifier_reference_list_t &scope_manager::unresolved_identifier_references()
{
    return unresolved_identifier_references_;
}

interned_string_literal_list_t &scope_manager::interned_string_literals()
{
    return interned_string_literals_;
}

module_stack_t &scope_manager::module_stack()
{
    return module_stack_;
}

}