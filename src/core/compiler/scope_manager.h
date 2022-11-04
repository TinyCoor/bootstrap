//
// Created by 12132 on 2022/10/21.
//

#ifndef BOOTSTRAP_SRC_CORE_COMPILER_SCOPE_MANAGER_H_
#define BOOTSTRAP_SRC_CORE_COMPILER_SCOPE_MANAGER_H_
#include "elements/element_types.h"
#include "core/parser/ast.h"
#include <stack>
#include <unordered_map>
#include <functional>
namespace gfx::compiler {
class session;
using block_stack_t = std::stack<block*>;
using module_stack_t = std::stack<module*>;

using block_visitor_callable = std::function<bool (compiler::block*)>;
using scope_visitor_callable = std::function<compiler::element* (compiler::block*)>;
using element_visitor_callable = std::function<compiler::element* (compiler::element*)>;
using namespace_visitor_callable = std::function<compiler::element* (compiler::block*)>;
using interned_string_literal_list_t = std::unordered_map<std::string, string_literal_list_t>;

class scope_manager {
public:
    explicit scope_manager(compiler::session &session);

    compiler::type* find_type(const qualified_symbol_t& symbol, compiler::block* scope = nullptr) const;

    compiler::block* current_top_level() const;

    compiler::module* current_module() const;

    class block* current_scope() const;

    bool visit_blocks(result& r, const block_visitor_callable& callable, compiler::block* root_block = nullptr);

    compiler::module* find_module(compiler::element* module);

    compiler::type* find_array_type(compiler::type* entry_type, size_t size, compiler::block* scope = nullptr) const;

    compiler::type* find_pointer_type(compiler::type* base_type, compiler::block* scope = nullptr) const;

    static element* walk_parent_scopes(compiler::block* scope, const scope_visitor_callable& callable) ;

    static element* walk_parent_elements(compiler::element* element, const element_visitor_callable& callable) ;

    element* walk_qualified_symbol(const qualified_symbol_t& symbol, compiler::block* scope,
        const namespace_visitor_callable& callable) const;

    bool find_identifier_type(type_find_result_t& result, const ast_node_shared_ptr& type_node,
                              compiler::block* parent_scope = nullptr);

    compiler::identifier* find_identifier(const qualified_symbol_t& symbol, compiler::block* scope = nullptr) const;

    compiler::type *make_complete_type(compiler::session& session, type_find_result_t &result,
        compiler::block *parent_scope) const;

    compiler::block* pop_scope();

    block_stack_t& top_level_stack();

    module_stack_t &module_stack();

    void push_scope(class block* block);

    void add_type_to_scope(compiler::type* value) const;

    bool within_procedure_scope(compiler::block* parent_scope) const;

    compiler::block* push_new_block(compiler::element_type_t type = element_type_t::block);

    unknown_type* unknown_type_from_result(compiler::block* scope, compiler::identifier* identifier,
       type_find_result_t& result);

    identifier_list_t &identifiers_with_unknown_types();

    identifier_reference_list_t &unresolved_identifier_references();

    interned_string_literal_list_t &interned_string_literals();
private:
    compiler::session& session_;
    block_stack_t top_level_stack_ {};
    block_stack_t scope_stack_ {};
    module_stack_t module_stack_ {};
    identifier_list_t identifiers_with_unknown_types_ {};
    identifier_reference_list_t unresolved_identifier_references_ {};
    interned_string_literal_list_t interned_string_literals_ {};
};
}
#endif //BOOTSTRAP_SRC_CORE_COMPILER_SCOPE_MANAGER_H_
