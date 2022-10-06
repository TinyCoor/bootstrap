//
// Created by 12132 on 2022/4/23.
//

#ifndef COMPILER_ELEMENTS_PROGRAM_H_
#define COMPILER_ELEMENTS_PROGRAM_H_
#include "block.h"
#include "element.h"
#include "parser/ast.h"
#include "../element_map.h"
#include "../element_builder.h"
#include "element_types.h"
#include "compiler/compiler_types.h"
#include "common/source_file.h"

namespace gfx::compiler {
struct type_find_result_t {
    qualified_symbol_t type_name{};
    bool is_array{false};
    bool is_pointer = false;
    bool is_spread = false;
    size_t array_size{0};
    compiler::type* type = nullptr;
};

class program : public element {
public:
    using block_visitor_callable = std::function<bool (compiler::block*)>;
    using scope_visitor_callable = std::function<compiler::element* (compiler::block*)>;
    using element_visitor_callable = std::function<compiler::element* (compiler::element*)>;
    using namespace_visitor_callable = std::function<compiler::element* (compiler::block*)>;

    program(terp* terp, assembler* assembler);

	~program() override;

    bool run(result& r);

    element_map& elements();

    void disassemble(FILE *file);

    compiler::block *block() const;

	bool compile(result& r, compiler::session& session);

    element_builder& builder();

    compiler::type* find_type(const qualified_symbol_t& symbol, compiler::block* scope = nullptr) const;

	module* compile_module(result& r, compiler::session& session, source_file *source);

    void error(result& r, compiler::element *element, const std::string &code,
               const std::string &message, const source_location& location);
protected:
	terp* terp();

    compiler::block* current_top_level();

    void error(result& r, compiler::session& session, const std::string &code,
               const std::string &message, const source_location& location);

private:
    bool on_emit(result& r, emit_context_t& context) override;

	void initialize_core_types(result &r);

    bool resolve_unknown_types(result& r);

    bool resolve_unknown_identifiers(result& r);

    bool visit_blocks(result& r, const block_visitor_callable& callable,
        compiler::block* root_block = nullptr);

private:
	void add_composite_type_fields(result& r, compiler::session& session, composite_type* struct_type, const ast_node_shared_ptr& block);

    void apply_attributes(result& r, compiler::session& session, compiler::element* element, const ast_node_shared_ptr& node);

	identifier * add_identifier_to_scope(result& r, compiler::session& session, symbol_element* symbol, type_find_result_t& find_type_result,
                                      const ast_node_shared_ptr& node, size_t source_index, compiler::block* parent_scope = nullptr);

    compiler::symbol_element* make_symbol_from_node(result& r, const ast_node_shared_ptr& node);

    void add_procedure_instance(result& r, compiler::session& session, compiler::procedure_type* proc_type, const ast_node_shared_ptr& node);

    static void add_expression_to_scope(compiler::block* scope, compiler::element* expr);

    void add_type_to_scope(compiler::type* value);

    compiler::element* resolve_symbol_or_evaluate(result& r, compiler::session& session,
        const ast_node_shared_ptr& node);

    bool within_procedure_scope(compiler::block* parent_scope) const;

    compiler::module* find_module(compiler::element* module) const;
private:
    friend class any_type;
    friend class bool_type;
    friend class directive;
    friend class tuple_type;
    friend class type_info;
    friend class array_type;
    friend class module_type;
    friend class string_type;
    friend class numeric_type;
    friend class pointer_type;
    friend class unary_operator;
    friend class namespace_type;
    friend class procedure_type;
    friend class binary_operator;
    friend class element_builder;

    static void make_qualified_symbol(qualified_symbol_t& symbol, const ast_node_shared_ptr& node);

	class compiler::block* push_new_block(compiler::element_type_t type = element_type_t::block);

	compiler::type* find_array_type(compiler::type* entry_type, size_t size, compiler::block* scope = nullptr) const;

    compiler::type* find_pointer_type(compiler::type* base_type, compiler::block* scope = nullptr) const;

    unknown_type* unknown_type_from_result(result& r, compiler::block* scope, compiler::identifier* identifier,
       type_find_result_t& result);
private:
	class block* pop_scope();

	class block* current_scope() const;

	void push_scope(class block* block);

    element* walk_parent_scopes(compiler::block* scope,
        const scope_visitor_callable& callable) const;

    element* walk_parent_elements(compiler::element* element,
        const element_visitor_callable& callable) const;

    element* walk_qualified_symbol(const qualified_symbol_t& symbol, compiler::block* scope,
        const namespace_visitor_callable& callable) const;

    element* evaluate(result& r, compiler::session& session, const ast_node_shared_ptr& node,
                      element_type_t default_block_type = element_type_t::block);

    element* evaluate_in_scope(result& r, compiler::session& session, const ast_node_shared_ptr& node,
        compiler::block* scope, element_type_t default_block_type = element_type_t::block);

    bool find_identifier_type(result& r, type_find_result_t& result, const ast_node_shared_ptr& type_node,
                              compiler::block* parent_scope = nullptr);

	compiler::identifier* find_identifier(const qualified_symbol_t& symbol, compiler::block* scope = nullptr) const;
    compiler::type* make_complete_type(result& r, type_find_result_t& result, compiler::block* parent_scope = nullptr);
private:
	assembler* assembler_ = nullptr;
	class terp* terp_ = nullptr;
    element_map elements_ {};
    element_builder builder_;
	compiler::block* block_ = nullptr;
    std::stack<compiler::block*> top_level_stack_;
	std::stack<compiler::block*> scope_stack_ {};
	identifier_list_t identifiers_with_unknown_types_ {};
    identifier_reference_list_t unresolved_identifier_references_ {};
    std::unordered_map<std::string, string_literal_list_t> interned_string_literals_ {};
};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
