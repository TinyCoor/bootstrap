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
#include "../ast_evaluator.h"

namespace gfx::compiler {

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

    static void error(result& r, compiler::element *element, const std::string &code,
               const std::string &message, const source_location& location);
protected:
	terp* terp();

    compiler::block* current_top_level();

    static void error(result& r, compiler::session& session, const std::string &code,
               const std::string &message, const source_location& location);

private:
    bool on_emit(result& r, emit_context_t& context) override;

	void initialize_core_types(result &r);

    bool resolve_unknown_types(result& r);

    bool resolve_unknown_identifiers(result& r);

    bool visit_blocks(result& r, const block_visitor_callable& callable,
        compiler::block* root_block = nullptr);

private:
    void add_type_to_scope(compiler::type* value);

    bool within_procedure_scope(compiler::block* parent_scope) const;

    static compiler::module* find_module(compiler::element* module) ;
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
    friend class symbol_element;
    friend class unary_operator;
    friend class namespace_type;
    friend class procedure_type;
    friend class binary_operator;
    friend class element_builder;
    friend class ast_evaluator;

    bool type_check(result& r, compiler::session& session);

	class compiler::block* push_new_block(compiler::element_type_t type = element_type_t::block);

	compiler::type* find_array_type(compiler::type* entry_type, size_t size, compiler::block* scope = nullptr) const;

    compiler::type* find_pointer_type(compiler::type* base_type, compiler::block* scope = nullptr) const;

    unknown_type* unknown_type_from_result(result& r, compiler::block* scope, compiler::identifier* identifier,
       type_find_result_t& result);
private:
	class block* pop_scope();

	class block* current_scope() const;

	void push_scope(class block* block);

    static element* walk_parent_scopes(compiler::block* scope,
        const scope_visitor_callable& callable) ;

    static element* walk_parent_elements(compiler::element* element,
        const element_visitor_callable& callable) ;

    element* walk_qualified_symbol(const qualified_symbol_t& symbol, compiler::block* scope,
        const namespace_visitor_callable& callable) const;


    bool find_identifier_type(result& r, type_find_result_t& result, const ast_node_shared_ptr& type_node,
                              compiler::block* parent_scope = nullptr);

	compiler::identifier* find_identifier(const qualified_symbol_t& symbol, compiler::block* scope = nullptr) const;
    compiler::type* make_complete_type(result& r, type_find_result_t& result, compiler::block* parent_scope = nullptr);
private:
	assembler* assembler_ = nullptr;
	class terp* terp_ = nullptr;
    element_builder builder_;
    element_map elements_ {};
	compiler::block* block_ = nullptr;
    ast_evaluator ast_evaluator_;
    std::stack<compiler::block*> top_level_stack_;
	std::stack<compiler::block*> scope_stack_ {};
	identifier_list_t identifiers_with_unknown_types_ {};
    identifier_reference_list_t unresolved_identifier_references_ {};
    std::unordered_map<std::string, string_literal_list_t> interned_string_literals_ {};
};
}

#endif // COMPILER_ELEMENTS_PROGRAM_H_
